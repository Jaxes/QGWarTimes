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
	// ����ƥ��ص�����ʧ�ܵ��ַ���
	std::vector<std::wstring> mMailbox;
	// �ͻ������Ƶ�socket��ӳ��
	std::map<std::wstring, SOCKET> mNameSocketMap;

	// �ص�����, �����ͻ��˵���
	std::map<std::wstring, std::function<std::wstring(std::wstring&, std::wistream&)>> mCallbacks;

	// ��������socket�������: Service
	explicit ServiceInterface(std::shared_ptr<Service> _In_ service);

	// �����ͻ�������, ��ָ���ͻ��˷���һ����
	WTRESULT Say(std::wstring const& ClientName, std::wstring const& what);
	// ������Ϸ����
	WTRESULT PushGameData(std::wstring const& ClientName, std::wstring const& what);
	// ���
	WTRESULT Shoot(std::wstring const& PlayerName, uint32_t Decrease);
	//��Ϸ��ʼ
	WTRESULT GameStart(int room, std::shared_ptr<WarTimes::CWarTimes>& pGame);
	//�������ͬ�����
	WTRESULT AllReady(int room, std::shared_ptr<WarTimes::CWarTimes>& pGame);
	//�����������Ϸ����
	WTRESULT GameEnding(int room, std::shared_ptr<WarTimes::CWarTimes>& pGame);
};

#endif //SERVICEINTERFACE_H