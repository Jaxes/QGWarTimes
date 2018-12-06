#include "ServiceInterface.h"

ServiceInterface::ServiceInterface(std::shared_ptr<Service> service) :
	mpService(service) {
	if(service == nullptr) {
		WTERROR("传入的服务端为空指针");
	} else {
		mpService->set_handle_function(
			[this](std::wstring& str, SOCKET sock)->std::wstring {
			std::wistringstream isstr(str);
			std::wstring funcName;
			getline(isstr, funcName);
			std::wstring clientName;
			getline(isstr, clientName);
			mNameSocketMap[clientName] = sock;
			auto findResult = mCallbacks.find(funcName);
			if(findResult != mCallbacks.end()) {
				return findResult->second(clientName, isstr);
			} else {
				mMailbox.push_back(std::move(str));
				WTWARN("收到未知数据");
			}
			return L"";
		});
	}

	mCallbacks[L"ClientInterface::Say"] =
		[this](std::wstring& name, std::wistream& win) -> std::wstring {
		WTRESULT wtr;
		size_t what_size;
		win >> what_size;
		win.ignore();
		std::wstring what;
		what.resize(what_size);
		win.read(&what[0], what_size * sizeof(wchar_t));
		std::wcout << L"客户端" << name << L"说: " << what << std::endl;
		wtr = Say(name, L"客户端" + name + L"说: " + what);
		if(wtr) {
			WTERROR("回应ClientInterface::Say失败: %s", WTStrErr(wtr));
		}
		return L"";
	};
}

WTRESULT ServiceInterface::Say(
	std::wstring const& ClientName,
	std::wstring const& what) {
	WTRESULT wtr;
	std::wostringstream osstr;
	osstr << __FUNCTIONW__ << std::endl;
	osstr << what.size() << std::endl;
	osstr << what << std::endl;
	auto NameSocket = mNameSocketMap.find(ClientName);
	if(NameSocket != mNameSocketMap.end()) {
		wtr = mpService->send(NameSocket->second, osstr.str());
		if(wtr == 0) {
			return 0;
		} else {
			mNameSocketMap.erase(NameSocket);
			return WTRESULT_SendFailed;
		}
	} else {
		return WTRESULT_UserNameError;
	}
}

// ③
WTRESULT ServiceInterface::PushGameData(
	std::wstring const& ClientName,
	std::wstring const& what) {
	WTRESULT wtr;
	std::wostringstream osstr;
	osstr << __FUNCTIONW__ << std::endl;
	osstr << what << std::endl;
	auto NameSocket = mNameSocketMap.find(ClientName);
	if(NameSocket != mNameSocketMap.end()) {
		wtr = mpService->send(NameSocket->second, osstr.str());
		if(wtr == 0) {
			return 0;
		} else {
			mNameSocketMap.erase(NameSocket);
			return WTRESULT_SendFailed;
		}
	} else {
		return WTRESULT_UserNameError;
	}
}

WTRESULT ServiceInterface::Shoot(std::wstring const& PlayerName, uint32_t Decrease)
{
	WTRESULT wtr;
	std::wostringstream osstr;
	osstr << __FUNCTIONW__ << std::endl;
	osstr << Decrease << std::endl;
	auto NameSocket = mNameSocketMap.find(PlayerName);
	if (NameSocket != mNameSocketMap.end()) {
		wtr = mpService->send(NameSocket->second, osstr.str());
		if (wtr == 0) {
			return WTRESULT_OK;
		}
		else {
			mNameSocketMap.erase(NameSocket);
			return WTRESULT_SendFailed;
		}
	}
	else {
		return WTRESULT_UserNameError;
	}
}

WTRESULT ServiceInterface::GameStart(int room, std::shared_ptr<WarTimes::CWarTimes>& pGame)
{
	WTRESULT wtr;
	std::wostringstream osstr;
	osstr << __FUNCTIONW__ << std::endl;
	std::wstring PlayerName;
	for (int i = 0; i < 2; i++)
	{
		for (auto& j : pGame->room[room-1]->team[i])
		{
			PlayerName = j.second->mName;
			auto NameSocket = mNameSocketMap.find(PlayerName);
			if (NameSocket != mNameSocketMap.end()) {
				wtr = mpService->send(NameSocket->second, osstr.str());
				if (wtr != 0) {
					mNameSocketMap.erase(NameSocket);
					return WTRESULT_SendFailed;
				}
			}
		}
	}
	return WTRESULT_OK;
}

WTRESULT ServiceInterface::AllReady(int room, std::shared_ptr<WarTimes::CWarTimes>& pGame)
{
	WTRESULT wtr;
	std::wostringstream osstr;
	osstr << __FUNCTIONW__ << std::endl;
	std::wstring PlayerName;
	for (int i = 0; i < 2; i++)
	{
		for (auto& j : pGame->room[room - 1]->team[i])
		{
			PlayerName = j.second->mName;
			auto NameSocket = mNameSocketMap.find(PlayerName);
			if (NameSocket != mNameSocketMap.end()) {
				wtr = mpService->send(NameSocket->second, osstr.str());
				if (wtr != 0) {
					mNameSocketMap.erase(NameSocket);
					return WTRESULT_SendFailed;
				}
			}
		}
	}
	return WTRESULT_OK;
}

WTRESULT ServiceInterface::GameEnding(int room, std::shared_ptr<WarTimes::CWarTimes>& pGame)
{
	WTRESULT wtr;
	std::wostringstream osstr;
	osstr << __FUNCTIONW__ << std::endl;
	std::wstring PlayerName;
	for (int i = 0; i < 2; i++)
	{
		for (auto& j : pGame->room[room - 1]->team[i])
		{
			PlayerName = j.second->mName;
			auto NameSocket = mNameSocketMap.find(PlayerName);
			if (NameSocket != mNameSocketMap.end()) {
				wtr = mpService->send(NameSocket->second, osstr.str());
				if (wtr != 0) {
					mNameSocketMap.erase(NameSocket);
					return WTRESULT_SendFailed;
				}
			}
		}
	}
	return WTRESULT_OK;
}
