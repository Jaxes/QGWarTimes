#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <mutex>
#include <functional>
#include <list>
#include <iostream>
#include <thread>

#include <WS2tcpip.h>
#include <WinSock2.h>
//#include <Windows.h>

#include "wterr.h"

class Client {
public:
	enum Status {
		Status_End,
		Status_OK,
		Status_Error,
	};

protected:
	bool isLock;
	bool writeable;
	Status status;

	SOCKET sock;
	WSAEVENT event;
	std::mutex write_lock;
	std::mutex event_lock;

	std::thread thread_find_event;
	std::thread thread_handle_event;

	int have_event;
	std::function<std::wstring(std::wstring& sstr, SOCKET socket)> handle_func;

protected:
	// 查找网络事件
	int find_event();
	// 处理事件
	int handle_event();

public:
	Client();
	int init(sockaddr const* addr);
	int run();
	int close();
	int send(std::wstring const& str); // 阻塞
	// 设置数据处理回调函数
	int set_handle_func(std::function<std::wstring(std::wstring& sstr, SOCKET socket)> handle_func);
	bool GetLocking();
	void SetLocking(bool isLock);

	~Client();
};

#endif //CLIENT_H