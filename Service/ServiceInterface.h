#ifndef SERVICEINTERFACE_H
#define SERVICEINTERFACE_H

#include <string>
#include <memory>
#include <iostream>
#include <sstream>
#include <functional>
#include <typeinfo>
#include <map>
#include <vector>
#include <set>

#include <WS2tcpip.h>
#include <WinSock2.h>
#include <Windows.h>

#include "Service.h"

#include "wtlog.h"
#include "wterr.h"
#include "WarTimes.h"

class ServiceInterface {
	ServiceInterface() = delete;
	ServiceInterface(ServiceInterface const&) = delete;
	ServiceInterface& operator= (ServiceInterface const&) = delete;

protected:
	std::shared_ptr<Service> mpService;

public:
	// 保存匹配回调函数失败的字符串
	std::vector<std::wstring> mMailbox;
	// 客户端名称到socket的映射
	std::map<std::wstring, SOCKET> mNameSocketMap;

	// 回调函数, 留给客户端调用
	std::map<std::wstring, std::function<std::wstring(std::wstring&, std::wistream&)>> mCallbacks;

	// 传入服务端socket管理对象: Service
	explicit ServiceInterface(std::shared_ptr<Service> _In_ service);

	// 给定客户端名称, 向指定客户端发送一行字
	WTRESULT Say(std::wstring const& ClientName, std::wstring const& what);
	// 推送游戏数据
	WTRESULT PushGameData(std::wstring const& ClientName, std::wstring const& what);
	// 射击
	WTRESULT Shoot(std::wstring const& PlayerName, uint32_t Decrease);
	//游戏开始
	WTRESULT GameStart(int room, std::shared_ptr<WarTimes::CWarTimes>& pGame);
	//玩家数据同步完成
	WTRESULT AllReady(int room, std::shared_ptr<WarTimes::CWarTimes>& pGame);
	//玩家死亡，游戏结束
	WTRESULT GameEnding(int room, std::shared_ptr<WarTimes::CWarTimes>& pGame);
};

#endif //SERVICEINTERFACE_H