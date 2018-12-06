#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <cassert>
#include <WinSock2.h>
#include <Windows.h>
#include <functional>
#include "wterr.h"
#include "WinTimer.h"

template<typename T>
class Animator {
	Animator() = delete;
	Animator(Animator const&) = delete;
	Animator& operator= (Animator const&) = delete;

protected:
	std::function<T(size_t frame)> mGenerator;
	std::function<void(T&&)> mHandler;
	WinTimer mTimer;

	size_t mTotal;
	size_t mNow;

public:
	// 创建动画
	explicit Animator(HWND _In_ hwnd);
	Animator(Animator&& other);
	// 初始化动画, 帧序数会作为参数输入给 generator 函数
	// generator 得到帧序数生成帧数据
	// 动画类将帧数据输入给 handler 函数进行处理
	// 间隔时间为 interval 单位为毫秒
	// total 为总帧数
	WTRESULT Init(std::function<T(size_t frame)>&& _In_ generator, std::function<void(T&&)>&& _In_ handler, UINT interval, size_t total);
	// 启动动画, 非阻塞, 可暂停
	WTRESULT Run();
	// 暂停动画
	WTRESULT Pause();
	//检测动画是否已经结束
	bool isEnd();

	~Animator();
};

// 创建动画
template<typename T>
inline Animator<T>::Animator(HWND hwnd) :
	mTimer(hwnd) {

}

template<typename T>
Animator<T>::Animator(Animator && other) :
	mGenerator(std::move(other.mGenerator)),
	mHandler(std::move(other.mHandler)),
	mTimer(std::move(other.mTimer)),
	mTotal(other.mTotal),
	mNow(other.mNow) {
	other.mGenerator = nullptr;
	other.mHandler = nullptr;
	other.mNow = 0;
	other.mTotal = 0;
}

// 启动动画, 非阻塞, 可反复暂停启动
template<typename T>
inline WTRESULT Animator<T>::Run() {
	ASSERT_RETURN(this->mTotal - this->mNow > 1, WTRESULT_AnimeIsEnd);
	return mTimer.Run();
}
 
// 暂停动画
template<typename T>
inline WTRESULT Animator<T>::Pause() {
	return mTimer.Stop();
}

template<typename T>
inline bool Animator<T>::isEnd() {
	if (this->mNow >= this->mTotal/*&&this->mTotal != 0*/)return true;
	else return false;
}

template<typename T>
inline Animator<T>::~Animator() {
	
}

// 初始化动画, 帧序数会作为参数输入给 generator 函数
// generator 得到帧序数生成帧数据
// 动画类将帧数据输入给 handler 函数进行处理
// 间隔时间为 interval 单位为毫秒
// total 为总帧数
template<typename T>
WTRESULT Animator<T>::Init(std::function<T(size_t frame)>&& _In_ generator, std::function<void(T&&)>&& _In_ handler, UINT _In_ interval, size_t _In_ total) {
	ASSERT_RETURN(generator != nullptr, WTRESULT_CallbackFunctionIsNull);
	ASSERT_RETURN(handler != nullptr, WTRESULT_CallbackFunctionIsNull);
	mGenerator = generator;
	mHandler = handler;
	mNow = 0;
	mTotal = total;
	return mTimer.Init(interval, [this](WinTimer*timer) {
		this->mHandler(this->mGenerator(mNow));
		if(++this->mNow >= this->mTotal) {
			timer->Stop();
		}
	});
}

#endif
