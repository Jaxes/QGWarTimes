/*
定时器, 给予HWND后, 设定回调函数和间隔时间并启动定时器后, 定时器可以周期性地调用回调函数
	
	启动一个定时器的例子:

	WinTimer timer(hwnd); // 创建计时器

	timer.Init(100, [](WinTimer*) {
		WTINFO("Hello!!!");
	} // 设定计时器

	timer.Run(); // 启动计时器

*/

#ifndef WINTIMER_H
#define WINTIMER_H
#include <Windows.h>
#include <mutex>
#include <functional>
#include <vector>
#include <cassert>

#include "wterr.h"

class WinTimer {
	friend void CALLBACK WinTimerProcess(HWND hwnd, UINT msg, UINT_PTR wParam, DWORD lParam);
	WinTimer& operator= (WinTimer const&) = delete;
	WinTimer(WinTimer const&) = delete;

protected:
	static std::mutex IDMapLock;
	static std::vector<WinTimer*> IDMap;
	static std::vector<size_t> FreeID;

protected:
	HWND mHwnd;
	UINT mInterval;
	UINT_PTR mID;
	std::function<void(WinTimer*)> mHandle;

public:
	// 装载窗口句柄
	explicit WinTimer(HWND _In_ hwnd);
	WinTimer(WinTimer &&);
	// 输入时间间隔(毫秒), 回调函数, 来初始化定时器
	WTRESULT Init(UINT _In_ interval, std::function<void(WinTimer*)>&& _In_ HandleFunc);
	// 非阻塞, 启动定时器
	WTRESULT Run();
	// 暂停定时器
	WTRESULT Stop();
	~WinTimer();
};

#endif
