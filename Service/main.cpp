#include <iostream>
#include <sstream>
#include <fstream>
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <Windows.h>
#include "wterr.h"
#include "InitSock.h"
#include "Service.h"
#include "Serialize.h"
#include "WarTimes.h"
#include "WarTimesObject.h"
#include "WarTimesWeapon.h"
#include "WarTimesPlayer.h"
#include "ServiceInterface.h"

CInitSock initSock;

std::wistream& operator>>(std::wistream& in, DirectX::XMFLOAT3& vec)
{
	in >> vec.x >> vec.y >> vec.z;
	return in;
}

std::wostream& operator<<(std::wostream& out, DirectX::XMINT3& vec)
{
	out << vec.x << L" " << vec.y << L" " << vec.z;
	return out;
}

std::wostream& operator<<(std::wostream& out, std::map<std::wstring, DirectX::XMINT3>& PlayerRoomInfo)
{
	for (auto i : PlayerRoomInfo)
	{
		out << i.first << L" " << i.second << std::endl;
	}
	return out;
}

int main(int argc, char** argv) {
	WTRESULT wtr;



	sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_port = htons(23320);
	InetPton(AF_INET, L"0.0.0.0", &addr.sin_addr);

	WTLOG_DYNAMIC_FILTER = WTLOG_INFO;
	setlocale(LC_CTYPE, "");
	pWTLogOutput = [](wchar_t const* wstr) {fputws(wstr, stdout);};
	WTINFO(L"Service 服务器启动了");

	// 创建游戏
	std::shared_ptr<WarTimes::CWarTimes> pGame(new WarTimes::CWarTimes());

	// 创建连接
	std::shared_ptr<Service> pServ(new Service());
	WTR(pServ->init(reinterpret_cast<sockaddr*>(&addr)));
	pServ->set_handle_function(
		[&](std::wstring str, SOCKET sock) -> std::wstring {
		std::wcout << L"收到: " << str << std::endl;return L"";
	});

	char buffer[256]{};
	char ipAddr[16]{};
	//客户端名字
	std::wstring Name;
	std::wstringstream ws;
	// 取得本地主机名称
	::gethostname(buffer, 256);
	// 通过主机名得到IP地址信息
	ADDRINFO* info;
	ADDRINFO hints{};
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	::getaddrinfo(buffer, nullptr, &hints, &info);
	sockaddr_in* pSin = (sockaddr_in*)info->ai_addr;
	inet_ntop(AF_INET, &pSin->sin_addr, ipAddr, sizeof(ipAddr));
	freeaddrinfo(info);
	
	std::ofstream out("ipAddr.txt",std::ios::trunc);
	out << ipAddr << std::endl;
	out.close();

	WTR(pServ->run());

	// 用ServiceInterface包装连接
	std::shared_ptr<ServiceInterface> pServiceHelper(new ServiceInterface(pServ));
	
	// 设置客户端可调用的函数
	// ②
	pServiceHelper->mCallbacks[L"ClientInterface::AddPlayer"] =
		[&](std::wstring& name, std::wistream& win) -> std::wstring {
		//更新到服务端
		int r, t, s;
		std::shared_ptr<WarTimes::CPlayer> pPlayer = std::dynamic_pointer_cast<WarTimes::CPlayer>(WarTimes::CPlayer::construct());
		win >> r >> t >> s;
		pPlayer->fromString(win);
		pGame->room[r - 1]->team[t - 1].insert(make_pair(s, pPlayer));
		pGame->PlayerRoomInfo[name] = { r,t,s };
		pGame->update();

		//同步到客户端
		std::wostringstream wosstr;
		pGame->toString(wosstr);
		std::wstring tmp_str = wosstr.str();
		pServiceHelper->PushGameData(name, tmp_str);
		WTINFO(L"玩家加入成功");

		if (pGame->room[r - 1]->PlayerNum == 2)
		{
			pServiceHelper->GameStart(r, pGame);
		}
		return L"";
	};

	pServiceHelper->mCallbacks[L"ClientInterface::AddRoom"] =
		[&](std::wstring& name, std::wistream& win) -> std::wstring {
		//更新到服务端
		pGame->AddRoom();

		//同步到客户端
		std::wostringstream wosstr;
		pGame->toString(wosstr);
		std::wstring tmp_str = wosstr.str();
		pServiceHelper->PushGameData(name, tmp_str);
		WTINFO(L"房间创建成功");
		return L"";
	};

	pServiceHelper->mCallbacks[L"ClientInterface::ExitRoom"] =
		[&](std::wstring& name, std::wistream& win) -> std::wstring {
		//更新到服务端
		int r;
		win >> r;
		if (pGame->room[r - 1]->PlayerNum == 0)
		{
			pGame->room.erase(pGame->room.begin() + (r - 1));
			for (auto& i : pGame->PlayerRoomInfo)
			{
				if (i.second.x > r)
				{
					i.second.x--;
				}
			}
		}

		//同步到客户端
		std::wostringstream wosstr;
		pGame->toString(wosstr);
		std::wstring tmp_str = wosstr.str();
		pServiceHelper->PushGameData(name, tmp_str);
		WTINFO(L"房间退出成功");
		return L"";
	};

	pServiceHelper->mCallbacks[L"ClientInterface::PullGameData"] =
		[&](std::wstring& name, std::wistream& win) -> std::wstring {
		//同步到客户端
		std::wostringstream wosstr;
		pGame->toString(wosstr);
		std::wstring tmp_str = wosstr.str();
		pServiceHelper->PushGameData(name, tmp_str);
		//WTINFO(L"拉取成功");
		return L"";
	};

	pServiceHelper->mCallbacks[L"ClientInterface::PushPlayerData"] =
		[&](std::wstring& name, std::wistream& win)->std::wstring {
		//更新到服务端
		int r, t, s;
		r = pGame->PlayerRoomInfo[name].x;
		t = pGame->PlayerRoomInfo[name].y;
		s = pGame->PlayerRoomInfo[name].z;
		pGame->room[r - 1]->team[t - 1][s]->fromString(win);

		//同步到客户端
		std::wostringstream wosstr;
		pGame->toString(wosstr);
		std::wstring tmp_str = wosstr.str();
		pServiceHelper->PushGameData(name, tmp_str);
		return L"";
	};

	pServiceHelper->mCallbacks[L"ClientInterface::PushReady"] =
		[&](std::wstring& name, std::wistream& win)->std::wstring {
		//更新到服务端
		int r;
		r = pGame->PlayerRoomInfo[name].x;
		pGame->room[r - 1]->ReadyNum++;

		if (pGame->room[r - 1]->ReadyNum == pGame->room[r - 1]->PlayerNum)
		{
			pServiceHelper->AllReady(r, pGame);
		}
		return L"";
	};

	pServiceHelper->mCallbacks[L"ClientInterface::Shoot"] = 
		[&](std::wstring& name, std::wistream& win)->std::wstring {
		//更新到服务端
		std::wstring Playername;
		uint32_t Decrease;
		win >> Playername >> Decrease;

		//同步到客户端
		std::wostringstream wosstr;
		pGame->toString(wosstr);
		std::wstring tmp_str = wosstr.str();
		pServiceHelper->PushGameData(name, tmp_str);

		//通知被攻击的客户端
		pServiceHelper->Shoot(Playername, Decrease);
		return L"";
	};

	pServiceHelper->mCallbacks[L"ClientInterface::GameEnding"] =
		[&](std::wstring& name, std::wistream& win)->std::wstring {
		int r;
		win >> r;

		//通知所有客户端
		pServiceHelper->GameEnding(r, pGame);
		return L"";
	};

	// 服务端的简易控制台
	std::wstring dat;
	std::map<std::wstring, std::function<void()>> cmds = {
		{L"say", [&] {
			std::wstring name;
			getline(std::wcin, name);
			getline(std::wcin, dat);
			wtr = pServiceHelper->Say(name, dat);
			if(wtr) {
				WTERROR("say: 发送失败: %s", WTStrErr(wtr));
			}
		}},
		{L"echo", [&] {
			getline(std::wcin, dat);
			std::wcout << dat << std::endl;
		}},
		{L"name", [&] {
			std::wcout << L"所有客户端名: ";
			for(auto const& i : pServiceHelper->mNameSocketMap) {
				std::wcout << i.first << ", ";
			}
			std::wcout << std::endl;
		}},
	};
	while(1) {
		getline(std::wcin, dat);
		auto find_result = cmds.find(dat);
		if(find_result != cmds.end()) {
			find_result->second();
		} else {
			std::wcout << L"未知命令, 可用命令: ";
			for(auto const& i : cmds) {
				std::wcout << i.first << L", ";
			}
			std::wcout << std::endl;
		}
	}
	// 不可能结束的
	std::wcout << L"结束" << std::endl;
	getchar();
	return 0;
}
