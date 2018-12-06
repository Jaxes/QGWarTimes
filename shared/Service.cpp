#include <iostream>
#include "service.h"
#include "wterr.h"

#undef ASSERT_ELSE

#define ASSERT_ELSE(__exp, __estr, __errno) \
	do{ \
		bool __res = (__exp); \
		if(!__res) { \
			int __errno_tmp = (__errno); \
			if(__errno_tmp) { \
				fprintf(stderr, "%s %d: %s: %s, 错误码: %d\r\n", \
					__func__, __LINE__, __estr, #__exp, __errno_tmp); \
			} else { \
				fprintf(stderr, "%s %d: %s: %s\r\n", \
					__func__, __LINE__, __estr, #__exp); \
			} \
		} \
	}while(0)

Service::Service() :
	status(Status_OK),
	handling([](std::wstring str, SOCKET sockt)->std::wstring {
		std::wcout << L"接收到: " << str << std::endl;
		return L"OK";
	}) {

}

int Service::init(sockaddr const* paddr) {
	int hr;
	SOCKET listen = socket(AF_INET, SOCK_STREAM, NULL);
	ASSERT_ELSE(listen != INVALID_SOCKET, "监听socket创建失败", (status = Status_Error, GetLastError()));
	WSAEVENT listen_event = WSACreateEvent();
	hr = bind(listen, paddr, sizeof(sockaddr));
	ASSERT_ELSE(hr != SOCKET_ERROR, "监听socket绑定失败", (status = Status_Error, GetLastError()));

	hr = WSAEventSelect(listen, listen_event, FD_ACCEPT | FD_CLOSE);
	ASSERT_ELSE(hr != SOCKET_ERROR, "监听socket的事件设置失败", (status = Status_Error, WSAGetLastError()));

	clients.push_back({ listen, false });
	clients_event.push_back(listen_event);
	return 0;
}

Service::~Service() {
	int hr;
	switch(status) {
	case Status_OK:
	case Status_End:
		status = Status_End;
		break;
	case Status_Error:
		std::clog << "Service 未知错误" << std::endl;
		break;
	default:
		std::clog << "Service 未知状态" << std::endl;
		break;
	}
	if(thread_find_events.joinable()) {
		thread_find_events.join();
	}
	if(thread_handle_events.joinable()) {
		thread_handle_events.join();
	}
	for(auto i : clients) {
		hr = closesocket(i.sock);
		ASSERT_ELSE(hr != SOCKET_ERROR, "socket销毁失败", GetLastError());
	}
	for(auto i : clients_event) {
		hr = WSACloseEvent(i);
		ASSERT_ELSE(hr == TRUE, "事件销毁失败", 0);
	}
}

int Service::run() {
	ASSERT_ELSE(status == Status_OK, "Service异常", 0);
	::listen(clients[0].sock, 16);
	// 事件处理要先布置现场
	thread_handle_events = std::thread(&Service::handle_events, this);
	Sleep(1);
	thread_find_events = std::thread(&Service::find_events, this);
	return 0;
}

void Service::find_events() {
	size_t index;
	ASSERT_ELSE(status == Status_OK, "Service异常", 0);
	while(status) {
		ASSERT_ELSE(clients.size() > 0, "事件空", 0);
		std::lock_guard<std::mutex> events_waiting_lock_helper(events_waiting_lock);
		clients_lock.lock();
		index = WSAWaitForMultipleEvents(static_cast<DWORD>(clients.size()), &clients_event[0], FALSE, 1, FALSE);
		clients_lock.unlock();
		index -= WSA_WAIT_EVENT_0;
		if(index == WSA_WAIT_FAILED || index == WSA_WAIT_TIMEOUT) {
			continue;
		}
		bool have = false;
		//events_waiting_lock.lock();
		for(auto const& i : events_waiting) {
			if(i.e == clients_event[index]) {
				have = true;
				break;
			}
		}
		//events_waiting_lock.unlock();
		if(!have) {
			WTDEBUG("index: %d, socket: %d 收集到事件", static_cast<int>(index), static_cast<int>(clients[index].sock));
			events_waiting.push_back({ clients[index].sock, clients_event[index] });
		}
	}
}

void Service::handle_events() {
	int hr;
	ASSERT_ELSE(status == Status_OK, "Service异常", 0);
	while(status) {
		if(events_waiting.size() == 0) {
			Sleep(1);
			continue;
		}
		while(events_waiting.size() > 0) {
			std::lock_guard<std::mutex> events_waiting_lock_helper(events_waiting_lock);
			EventUnit const& event_unit = events_waiting.front();
			SOCKET sock = event_unit.sock;
			WSAEVENT wsae = event_unit.e;
			WSANETWORKEVENTS e = { 0 };
			WSAEnumNetworkEvents(sock, wsae, &e);
			//events_waiting_lock.lock();
			events_waiting.pop_front();
			//events_waiting_lock.unlock();
			if(e.lNetworkEvents & FD_ACCEPT) { // 新连接接入
				WTDEBUG("socket: %d accept", static_cast<int>(sock));
				ASSERT_ELSE(e.iErrorCode[FD_ACCEPT_BIT] == 0, "accept出错", (status = Status_Error, e.iErrorCode[FD_ACCEPT_BIT]));
				if(clients.size() >= WSA_MAXIMUM_WAIT_EVENTS + 1) {
					std::cout << "accept过多" << std::endl;
				} else {
					SOCKET new_client = accept(sock, nullptr, nullptr);
					ASSERT_ELSE(new_client != INVALID_SOCKET, "异常的远端socket", GetLastError());
					WSAEVENT new_event = WSACreateEvent();
					hr = WSAEventSelect(new_client, new_event, FD_READ | FD_CLOSE | FD_WRITE);
					ASSERT_ELSE(hr != SOCKET_ERROR, "为远端socket设置事件失败", GetLastError());
					clients.push_back({ new_client, false });
					clients_event.push_back(new_event);
				}
			}
			if(e.lNetworkEvents & FD_READ) { // 新数据
				WTDEBUG("socket: %d read", static_cast<int>(sock));
				ASSERT_ELSE(e.iErrorCode[FD_READ_BIT] == 0, "read出错", (status = Status_Error, e.iErrorCode[FD_READ_BIT]));
				buf.resize(1024);
				DWORD netErrNo;
				while(1) {
					hr = ::recv(sock, reinterpret_cast<char*>(&buf[0]), 1024 * sizeof(wchar_t), 0);
					switch(hr) {
					case SOCKET_ERROR:
						netErrNo = GetLastError();
						switch(netErrNo) {
						default:
							WTERROR("套接字接收错误: %d", netErrNo);
							break;
						}
					case 0:
						break;
					case 1024 * sizeof(wchar_t):
						buf.resize(buf.size() + 1024);
						continue;
					default:
						buf.resize(buf.size() - 1024 + (hr + 1) / sizeof(wchar_t));
						WTDEBUG(L"套接字%d 读入%d字节: \"%s\"", sock, hr, buf.c_str());
						{
							std::wstring acq = handling(buf, sock);
							if(acq.size() > 0) {
								hr = ::send(sock, reinterpret_cast<const char*>(acq.c_str()), acq.size() * sizeof(wchar_t), 0);
								if(hr == SOCKET_ERROR) {
									WTERROR("套接字%d 回复失败: %d", sock, GetLastError());
								}
							}
						}
						break;
					}
					break;
				}
			}
			if(e.lNetworkEvents & FD_WRITE) { // 可写入
				WTDEBUG("socket: %d write", static_cast<int>(sock));
				ASSERT_ELSE(e.iErrorCode[FD_WRITE_BIT] == 0, "write出错", (status = Status_Error, e.iErrorCode[FD_WRITE_BIT]));
				for(auto & i : clients) {
					if(i.sock == sock) {
						i.writeable = true;
						break;
					}
				}
			}
			if(e.lNetworkEvents & FD_CLOSE) { // 连接关闭
				WTDEBUG("socket: %d close", static_cast<int>(sock));
				switch(e.iErrorCode[FD_CLOSE_BIT]) {
				default:
					ASSERT_ELSE(e.iErrorCode[FD_CLOSE_BIT] == 0, "close出错", e.iErrorCode[FD_CLOSE_BIT]);
				case 10038:
				case 10054:
					WTDEBUG("socket: %d close(%d)", static_cast<int>(sock), e.iErrorCode[FD_CLOSE_BIT]);
				case 0:
					clients_lock.lock();
					hr = closesocket(sock);
					ASSERT_ELSE(hr != SOCKET_ERROR, "socket关闭失败", GetLastError());
					hr = WSACloseEvent(wsae);
					ASSERT_ELSE(hr == TRUE, "事件关闭失败", 0);
					hr = 1;
					for(auto i = clients.begin(), end = clients.end(); i != end; i++) {
						if(i->sock == sock) {
							clients.erase(i);
							hr = 0;
							break;
						}
					}
					ASSERT_ELSE(hr == 0, "socket句柄丢失", 0);
					hr = 1;
					for(auto i = clients_event.begin(), end = clients_event.end(); i != end; i++) {
						if(*i == wsae) {
							clients_event.erase(i);
							hr = 0;
							break;
						}
					}
					clients_lock.unlock();
					ASSERT_ELSE(hr == 0, "事件句柄丢失", 0);
				}
			}
		}
	}
}

int Service::set_handle_function(std::function<std::wstring(std::wstring&, SOCKET sock)> handle_func) {
	this->handling = handle_func;
	return 0;
}

unsigned Service::get_sock_port(unsigned & port) {
	ASSERT_ELSE(status == Status_OK, "Service异常", 0);
	sockaddr_in addr;
	int addr_len = sizeof(addr);
	getsockname(clients[0].sock, (sockaddr*)&addr, &addr_len);
	port = ntohs(addr.sin_port);
	return port;
}

void Service::join() {
	if(thread_find_events.joinable()) {
		thread_find_events.join();
	}
	if(thread_handle_events.joinable()) {
		thread_handle_events.join();
	}
}

int Service::send(SOCKET sock, std::wstring const & str) {
	bool found = false;
	for(size_t i = 1; i < clients.size(); i++) {
		if((sock == 0 || clients[i].sock == sock) && clients[i].writeable) {
			found = true;
			::send(clients[i].sock, reinterpret_cast<const char*>(str.c_str()), str.size() * sizeof(wchar_t), 0);
		}
	}
	if(found) {
		return 0;
	} else {
		return -1;
	}
}

Service::Status Service::get_status() {
	return this->status;
}

int Service::close() {
	ASSERT_ELSE(status == Status_OK, "Service异常", 0);
	status = Status_End;
	return 0;
}
