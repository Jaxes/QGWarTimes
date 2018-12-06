#pragma once

#include <WinSock2.h>
#include <string>
#include <vector>
#include <list>
#include <array>
#include <thread>
#include <mutex>
#include <future>
#include <functional>

class Service {
public:
	enum Status {
		Status_End,
		Status_OK,
		Status_Error,
	};

	struct LinkUnit {
		SOCKET sock;
		bool writeable;
	};

	struct EventUnit {
		SOCKET sock;
		WSAEVENT e;
	};

protected:
	Status status;

	std::mutex clients_lock;
	std::vector<LinkUnit> clients; // 第一个是监听,其余为连接
	std::vector<WSAEVENT> clients_event; // 第一个是监听,其余为连接
	std::thread thread_find_events; // 事件发现线程

	std::mutex events_waiting_lock;
	std::list<EventUnit> events_waiting; // 待处理事件队列
	std::function<std::wstring(std::wstring&, SOCKET sock)> handling; // 事件处理回调函数
	std::thread thread_handle_events; // 事件处理线程

	std::wstring buf;

public:
	Service();
	Service(Service&) = delete;
	Service& operator= (Service&) = delete;
	~Service();

	int init(sockaddr const* paddr);
	int run(); // 多线程, 非阻塞
	void find_events();
	void handle_events();
	int set_handle_function(std::function<std::wstring(std::wstring&, SOCKET sock)> handle_func);
	unsigned get_sock_port(unsigned& port);
	void join();
	int send(SOCKET sock, std::wstring const& str); // 阻塞
	Status get_status();
	int close();
};
