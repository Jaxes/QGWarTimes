#include "WinTimer.h"

std::mutex WinTimer::IDMapLock;
std::vector<WinTimer*> WinTimer::IDMap(1);
std::vector<size_t> WinTimer::FreeID;

static void CALLBACK
WinTimerProcess(HWND hwnd, UINT msg, UINT_PTR timerID, DWORD lParam) {
	WTDEBUG("定时器%d触发", timerID);
	WinTimer* self;
	{
		std::lock_guard<std::mutex> auto_lock(WinTimer::IDMapLock);
		self = WinTimer::IDMap[timerID];
	}
	assert(self != NULL);
	self->mHandle(self);
}

WinTimer::WinTimer(HWND hwnd) :
	mHwnd(hwnd),
	mID(0),
	mInterval(0),
	mHandle(nullptr) {
	ASSERT_ELSE(hwnd != 0, WTRESULT_CallbackFunctionIsNull);
}

WinTimer::WinTimer(WinTimer && other) :
	mHandle(std::move(other.mHandle)),
	mHwnd(other.mHwnd),
	mInterval(other.mInterval),
	mID(other.mID) {
	other.mHandle = nullptr;
	other.mHwnd = 0;
	other.mID = 0;
	other.mInterval = 0;
}

WTRESULT WinTimer::Init(UINT interval, std::function<void(WinTimer*)>&& HandleFunc) {
	mHandle = HandleFunc;
	mInterval = interval;
	ASSERT_RETURN(mHwnd != 0, WTRESULT_HwndIsNull);
	ASSERT_RETURN(interval > 0, WTRESULT_IntervalTooSmall);
	{
		std::lock_guard<std::mutex> auto_lock(IDMapLock);

		if(FreeID.size() > 0) {
			mID = FreeID.back();
			FreeID.pop_back();
		} else {
			mID = IDMap.size();
			IDMap.resize(mID + 1);
		}
		IDMap[mID] = this;
	}
	return 0;
}

WTRESULT WinTimer::Run() {
	ASSERT_RETURN(mHwnd != 0, WTRESULT_TimerIsNotRunning);
	ASSERT_RETURN(mHwnd != 0, WTRESULT_HwndIsNull);
	ASSERT_RETURN(mInterval > 0, WTRESULT_IntervalTooSmall);
	UINT_PTR res = SetTimer(mHwnd, mID, mInterval, WinTimerProcess);
	WTDEBUG(L"res == %d, mID == %d", res, mID);
	ASSERT_RETURN(res == mID, WTRESULT_TimerCreateFailed);
	ASSERT_RETURN(res != 0, WTRESULT_TimerCreateFailed);
	return 0;
}

WTRESULT WinTimer::Stop() {
	ASSERT_RETURN(mID != 0, WTRESULT_TimerIsNotRunning);
	BOOL res = KillTimer(mHwnd, mID);
	//ASSERT_RETURN(res == TRUE, WTRESULT_TimerKillFailed);
	return 0;
}

WinTimer::~WinTimer() {
	if(!mHwnd)return;
	WTRESULT wtr = Stop();
	{
		std::lock_guard<std::mutex> auto_lock(IDMapLock);

		IDMap[mID] = nullptr;
		FreeID.push_back(mID);
	}
}
