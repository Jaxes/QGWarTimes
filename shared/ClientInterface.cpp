#include "ClientInterface.h"

ClientInterface::ClientInterface(std::shared_ptr<Client> _In_ client, std::wstring name) :
	mpClient(client),
	mName(name)
{
	if(client == nullptr) {
		WTERROR("传入的客户端为空指针");
	} else {
		client->set_handle_func(
			[this](std::wstring& str, SOCKET socket) -> std::wstring {
			std::wistringstream isstr(str);
			std::wstring funcName;
			getline(isstr, funcName);
			auto findResult = mCallbacks.find(funcName);
			if(findResult != mCallbacks.end()) {
				return findResult->second(isstr);
			} else {
				mMailbox.push_back(std::move(str));
				WTWARN("收到未知数据");
			}
			return L"";
		});
	}

	mCallbacks[L"ServiceInterface::Say"] =
		[](std::wistream& win) -> std::wstring {
		size_t what_size;
		win >> what_size;
		win.ignore();
		std::wstring what;
		what.resize(what_size);
		win.read(&what[0], what_size * sizeof(wchar_t));
		std::wcout << L"服务端说: " << what << std::endl;
		return L"";
	};
}

WTRESULT ClientInterface::Say(std::wstring const& what) {
	WTRESULT wtr;
	std::wostringstream osstr;
	osstr << __FUNCTIONW__ << std::endl;
	osstr << mName << std::endl;
	osstr << what.size() << std::endl;
	osstr << what << std::endl;
	wtr = mpClient->send(osstr.str());
	ASSERT_RETURN(wtr == 0, WTRESULT_SendFailed);
	return WTRESULT_OK;
}

// ①
WTRESULT ClientInterface::PullGameData() {
	WTRESULT wtr;
	std::wostringstream osstr;
	osstr << __FUNCTIONW__ << std::endl;
	osstr << mName << std::endl;
	wtr = mpClient->send(osstr.str());
	mpClient->SetLocking(false);
	ASSERT_RETURN(wtr == 0, WTRESULT_SendFailed);
	//int sleepNum = 0;
	//while (!mpClient->GetLocking())
	//{
	//	if (sleepNum >= 50)
	//	{
	//		/*wtr = mpClient->send(osstr.str());
	//		ASSERT_RETURN(wtr == 0, WTRESULT_SendFailed);*/
	//		break;
	//	}
	//	std::this_thread::sleep_for(std::chrono::milliseconds(5));
	//	sleepNum++;
	//}
	while (!mpClient->GetLocking())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	mpClient->SetLocking(false);
	return WTRESULT_OK;
}

WTRESULT ClientInterface::AddPlayer(int room, int team, int seat, std::shared_ptr<WarTimes::CWarTimes>& pGame) {
	WTRESULT wtr;
	std::wostringstream osstr;
	osstr << __FUNCTIONW__ << std::endl;
	osstr << mName << std::endl;
	osstr << room << " " << team << " " << seat << std::endl;
	pGame->room[room - 1]->team[team - 1][seat] = std::dynamic_pointer_cast<WarTimes::CPlayer>(WarTimes::CPlayer::construct());
	pGame->room[room - 1]->team[team - 1][seat]->mName = mName;
	pGame->room[room - 1]->team[team - 1][seat]->toString(osstr);
	wtr = mpClient->send(osstr.str());
	mpClient->SetLocking(false);
	ASSERT_RETURN(wtr == 0, WTRESULT_SendFailed);
	//int sleepNum = 0;
	//while (!mpClient->GetLocking())
	//{
	//	if (sleepNum >= 80)
	//	{
	//		/*wtr = mpClient->send(osstr.str());
	//		ASSERT_RETURN(wtr == 0, WTRESULT_SendFailed);*/
	//		break;
	//	}
	//	std::this_thread::sleep_for(std::chrono::milliseconds(5));
	//	sleepNum++;
	//}
	while (!mpClient->GetLocking())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	mpClient->SetLocking(false);
	return WTRESULT_OK;
}

WTRESULT ClientInterface::AddRoom()
{
	WTRESULT wtr;
	std::wostringstream osstr;
	osstr << __FUNCTIONW__ << std::endl;
	osstr << mName << std::endl;
	wtr = mpClient->send(osstr.str());
	mpClient->SetLocking(false);
	ASSERT_RETURN(wtr == 0, WTRESULT_SendFailed);
	//int sleepNum = 0;
	//while (!mpClient->GetLocking())
	//{
	//	if (sleepNum >= 80)
	//	{
	//		/*wtr = mpClient->send(osstr.str());
	//		ASSERT_RETURN(wtr == 0, WTRESULT_SendFailed);*/
	//		break;
	//	}
	//	std::this_thread::sleep_for(std::chrono::milliseconds(5));
	//	sleepNum++;
	//}
	while (!mpClient->GetLocking())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	mpClient->SetLocking(false);
	return WTRESULT_OK;
}

WTRESULT ClientInterface::ExitRoom(int room)
{
	WTRESULT wtr;
	std::wostringstream osstr;
	osstr << __FUNCTIONW__ << std::endl;
	osstr << mName << std::endl;
	osstr << room << std::endl;
	wtr = mpClient->send(osstr.str());
	mpClient->SetLocking(false);
	ASSERT_RETURN(wtr == 0, WTRESULT_SendFailed);
	//int sleepNum = 0;
	//while (!mpClient->GetLocking())
	//{
	//	if (sleepNum >= 80)
	//	{
	//		/*wtr = mpClient->send(osstr.str());
	//		ASSERT_RETURN(wtr == 0, WTRESULT_SendFailed);*/
	//		break;
	//	}
	//	std::this_thread::sleep_for(std::chrono::milliseconds(5));
	//	sleepNum++;
	//}
	while (!mpClient->GetLocking())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	mpClient->SetLocking(false);
	return WTRESULT_OK;
}

WTRESULT ClientInterface::PushPlayerData(std::shared_ptr<WarTimes::CWarTimes>& pGame, std::shared_ptr<Sprite3D> Man, std::shared_ptr<Sprite3D> Gun)
{
	WTRESULT wtr;
	std::wostringstream osstr;
	osstr << __FUNCTIONW__ << std::endl;
	osstr << mName << std::endl;
	int r, t, s;
	r = pGame->PlayerRoomInfo[mName].x;
	t = pGame->PlayerRoomInfo[mName].y;
	s = pGame->PlayerRoomInfo[mName].z;
	Man->DataSync(pGame->room[r - 1]->team[t - 1][s]);
	Gun->DataSync(pGame->room[r - 1]->team[t - 1][s]->mpWeapon);
	pGame->room[r - 1]->team[t - 1][s]->toString(osstr);
	wtr = mpClient->send(osstr.str());
	mpClient->SetLocking(false);
	ASSERT_RETURN(wtr == 0, WTRESULT_SendFailed);
	//int sleepNum = 0;
	//while (!mpClient->GetLocking())
	//{
	//	if (sleepNum >= 80)
	//	{
	//		/*wtr = mpClient->send(osstr.str());
	//		ASSERT_RETURN(wtr == 0, WTRESULT_SendFailed);*/
	//		break;
	//	}
	//	std::this_thread::sleep_for(std::chrono::milliseconds(5));
	//	sleepNum++;
	//}
	while (!mpClient->GetLocking())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	mpClient->SetLocking(false);
	return WTRESULT_OK;
}

WTRESULT ClientInterface::PushReady()
{
	WTRESULT wtr;
	std::wostringstream osstr;
	std::wstring PlayerData;
	osstr << __FUNCTIONW__ << std::endl;
	osstr << mName << std::endl;
	wtr = mpClient->send(osstr.str());
	mpClient->SetLocking(false);
	ASSERT_RETURN(wtr == 0, WTRESULT_SendFailed);
	//int sleepNum = 0;
	//while (!mpClient->GetLocking())
	//{
	//	if (sleepNum >= 80)
	//	{
	//		/*wtr = mpClient->send(osstr.str());
	//		ASSERT_RETURN(wtr == 0, WTRESULT_SendFailed);*/
	//		break;
	//	}
	//	std::this_thread::sleep_for(std::chrono::milliseconds(5));
	//	sleepNum++;
	//}
	while (!mpClient->GetLocking())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	mpClient->SetLocking(false);
	return WTRESULT_OK;
}

WTRESULT ClientInterface::Shoot(std::wstring PlayerName,uint32_t Decrease)
{
	WTRESULT wtr;
	std::wostringstream osstr;
	std::wstring PlayerData;
	osstr << __FUNCTIONW__ << std::endl;
	osstr << mName << std::endl;
	osstr << PlayerName << " " << Decrease << std::endl;
	wtr = mpClient->send(osstr.str());
	mpClient->SetLocking(false);
	ASSERT_RETURN(wtr == 0, WTRESULT_SendFailed);
	while (!mpClient->GetLocking())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	mpClient->SetLocking(false);
	return WTRESULT_OK;
}

WTRESULT ClientInterface::GameEnding(int room)
{
	WTRESULT wtr;
	std::wostringstream osstr;
	std::wstring PlayerData;
	osstr << __FUNCTIONW__ << std::endl;
	osstr << mName << std::endl;
	osstr << room << std::endl;
	wtr = mpClient->send(osstr.str());
	mpClient->SetLocking(false);
	ASSERT_RETURN(wtr == 0, WTRESULT_SendFailed);
	return WTRESULT_OK;
}