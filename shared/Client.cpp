#include "Client.h"


Client::Client() :
	status(Status_OK),
	have_event(0),
	event(),
	handle_func([](std::wstring& sstr, SOCKET sock) -> std::wstring {
		return L"";
	}) {

}

int Client::init(sockaddr const* addr) {
	int hr;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	ASSERT_ELSE(sock != INVALID_SOCKET, GetLastError());
	ASSERT_RETURN(sock != INVALID_SOCKET, (status = Status_Error, WTRESULT_CreateSocketError));
	hr = connect(sock, addr, sizeof(sockaddr));
	ASSERT_ELSE(hr != SOCKET_ERROR, GetLastError());
	ASSERT_RETURN(hr != SOCKET_ERROR, (status = Status_Error, WTRESULT_ConnectingTimeOut));
	ASSERT_ELSE(hr == 0, GetLastError()); // 真·未知错误
	event = WSACreateEvent();
	hr = WSAEventSelect(sock, event, FD_READ | FD_WRITE | FD_CLOSE | FD_CONNECT);
	ASSERT_ELSE(hr != SOCKET_ERROR, GetLastError());
	ASSERT_RETURN(hr != SOCKET_ERROR, (status = Status_Error, WTRESULT_EventSettingError));
	isLock = false;
	return 0;
}

int Client::run() {
	ASSERT_RETURN(status == Status_OK, WTRESULT_ClientError);
	if(thread_find_event.joinable()) {
		return WTRESULT_ClientIsRunning;
	}
	thread_find_event = std::thread(&Client::find_event, this);
	if(thread_handle_event.joinable()) {
		return WTRESULT_ClientIsRunning;
	}
	thread_handle_event = std::thread(&Client::handle_event, this);
	return 0;
}

int Client::close() {
	ASSERT_RETURN(status == Status_OK, WTRESULT_ClientError);
	status = Status_End;
	return 0;
}

int Client::send(std::wstring const& str) {
	int hr;
	ASSERT_RETURN(status == Status_OK, WTRESULT_ClientError);
	while(status) {
		if(writeable) {
			{
				std::lock_guard<std::mutex> auto_lock(write_lock);
				if(writeable) {
					hr = ::send(sock, reinterpret_cast<const char*>(str.c_str()), str.size() * sizeof(wchar_t), 0);
				} else {
					return WTRESULT_SendFailed;
				}
			}
			if(hr == SOCKET_ERROR) {
				writeable = false;
				return WTRESULT_SendFailed;
			}
			return 0;
		} else {
			return WTRESULT_SendFailed;
		}
	}
	return 0;
}

int Client::set_handle_func(std::function<std::wstring(std::wstring& str, SOCKET sock)> handle_func) {
	this->handle_func = handle_func;
	return 0;
}

bool Client::GetLocking()
{
	return isLock;
}

void Client::SetLocking(bool isLock)
{
	this->isLock = isLock;
}

int Client::find_event() {
	ASSERT_RETURN(status == Status_OK, WTRESULT_ClientError);
	while(status) {
		if(have_event) {
			Sleep(1);
			continue;
		}
		DWORD index = WSAWaitForMultipleEvents(1, &event, FALSE, 1, FALSE);
		index -= WSA_WAIT_EVENT_0;
		event_lock.lock();
		if (index != WSA_WAIT_TIMEOUT)
		{
			have_event = true;
		}		
		event_lock.unlock();
	}
	return 0;
}

int Client::handle_event() {
	int hr;
	ASSERT_RETURN(status == Status_OK, WTRESULT_ClientError);
	std::wstring buf;
	while(status) {
		if(!have_event) {
			Sleep(1);
			continue;
		}
		WSANETWORKEVENTS e;
		event_lock.lock();
		WSAEnumNetworkEvents(sock, event, &e);
		have_event = false;
		event_lock.unlock();
		if(e.lNetworkEvents & FD_READ) {
			WTDEBUG("套接字%d 试读", static_cast<int>(sock));
			if(e.iErrorCode[FD_READ_BIT] != 0) {
				status = Status_Error;
				WTERROR("套接字%d 接收错误: %d", static_cast<int>(sock), e.iErrorCode[FD_READ_BIT]);
			}
			buf.resize(1024);
			while(1) {
				hr = recv(sock, reinterpret_cast<char*>(&buf[buf.size() - 1024]), 1024 * sizeof(wchar_t), 0);
				switch(hr) {
				case SOCKET_ERROR:
					WTERROR("套接字%d 接收错误: %d", static_cast<int>(sock), GetLastError());
				case 0:
					break;
				case 1024 * sizeof(wchar_t):
					buf.resize(buf.size() + 1024);
					continue;
				default:
					buf.resize(buf.size() - 1024 + (hr + 1) / sizeof(wchar_t));
					WTDEBUG(L"套接字%d 读入%d字节: \"%s\"", static_cast<int>(sock), hr, buf.c_str());
					{
						std::wstring acq = handle_func(buf, sock);
						if(acq.size() > 0) {
							hr = ::send(sock, reinterpret_cast<const char*>(acq.c_str()), acq.size() * sizeof(wchar_t), 0);
							if(hr == SOCKET_ERROR) {
								WTERROR("套接字%d 回复失败: %d", static_cast<int>(sock), GetLastError());
							}
						}
					}
					break;
				}
				break;
			}
			isLock = true;
		}
		if(e.lNetworkEvents & FD_WRITE) {
			WTDEBUG("套接字%d 可写", static_cast<int>(sock));
			writeable = true;
		}
		if(e.lNetworkEvents & FD_CLOSE) {
			WTDEBUG("套接字%d 关闭", static_cast<int>(sock));
			switch(e.iErrorCode[FD_CLOSE_BIT]) {
			default:
				WTDEBUG("套接字%d 出错, 错误码%d", static_cast<int>(sock), e.iErrorCode[FD_CLOSE_BIT]);
			case 10038:
			case 10054:
				WTDEBUG("套接字%d 关闭", static_cast<int>(sock));
			case 0:
				status = Status_End;
				break;
			}
		}
	}
	return 0;
}

Client::~Client() {
	int hr;
	switch(status) {
	case Status_OK:
	case Status_End:
		status = Status_End;
		break;
	case Status_Error:
		WTERROR("客户端错误");
		break;
	default:
		WTERROR("客户端状态异常");
		break;
	}
	if(thread_find_event.joinable()) {
		WTDEBUG("等待查询线程运行结束");
		thread_find_event.join();
		WTDEBUG("查询线程运行结束");
	}
	if(thread_handle_event.joinable()) {
		WTDEBUG("等待处理线程运行结束");
		thread_handle_event.join();
		WTDEBUG("处理线程运行结束");
	}
	hr = closesocket(sock);
	if(hr == SOCKET_ERROR) {
		WTERROR("销毁套接字失败, 错误码: %d", GetLastError(), hr);
	}
	hr = WSACloseEvent(event);
	if(hr == FALSE) {
		WTERROR("销毁事件失败, 错误码: %d", WSAGetLastError(), hr);
	}
}
