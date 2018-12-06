#ifndef CLIENTINTERFACE_H
#define CLIENTINTERFACE_H
/*
客户端服务端函数调用助手
数据一行一行发送,行行间换行符分隔

发送的第一行数据表示调用方的函数名
被调用方的mCallbacks要以调用方的函数名为键设置好回调函数
否则调用方会调用失败

客户端发送的第二行要表示客户端的名字

其余的由调用方和被调用方协调
*/

#include <string>
#include <memory>
#include <iostream>
#include <sstream>
#include <functional>
#include <typeinfo>
#include <map>
#include <vector>
#include <set>
#include <DirectXMath.h>

#include "Client.h"

#include "wtlog.h"
#include "wterr.h"
#include "WarTimesPlayer.h"
#include "Sprite3D.h"
#include "WarTimesWeapon.h"

class ClientInterface {
	ClientInterface() = delete;
	ClientInterface(ClientInterface const&) = delete;
	ClientInterface& operator= (ClientInterface const&) = delete;

protected:
	std::wstring mName;
	std::shared_ptr<Client> mpClient;

public:
	// 保存匹配回调函数失败的字符串
	std::vector<std::wstring> mMailbox;
	// 回调函数, 留给服务端调用
	// 键要填服务端的函数名
	std::map<std::wstring, std::function<std::wstring(std::wistream&)>> mCallbacks;

	// Client 先 init 好 run 好再传进来
	explicit ClientInterface(std::shared_ptr<Client> _In_ client, std::wstring name);

	// 向服务器发送字符串
	WTRESULT Say(std::wstring const& what);
	// 拉取游戏数据
	// 服务端会调用客户端 L"ServiceInterface::PushGameData" 的回调函数
	WTRESULT PullGameData();
	// 加入房间
	WTRESULT AddPlayer(int room, int team, int seat, std::shared_ptr<WarTimes::CWarTimes>& pGame);
	//创建房间
	WTRESULT AddRoom();
	//退出房间
	WTRESULT ExitRoom(int room);
	//上传玩家数据
	WTRESULT PushPlayerData(std::shared_ptr<WarTimes::CWarTimes>& pGame, std::shared_ptr<Sprite3D> Man, std::shared_ptr<Sprite3D> Gun);
	//所有玩家加载成功
	WTRESULT PushReady();
	//射击
	WTRESULT Shoot(std::wstring PlayerName, uint32_t Decrease);
	//游戏结束
	WTRESULT GameEnding(int room);
};

#endif //CLIENTINTERFACE_H