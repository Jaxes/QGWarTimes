#include "InitSock.h"

CInitSock::CInitSock(BYTE minorVer, BYTE majorVer) {
	WSADATA wsaData;
	//std::cout << "加载 Winsock 中" << std::endl;
	int rtn = WSAStartup(MAKEWORD(minorVer, majorVer), &wsaData);
	if(rtn) {
		std::cout << "Winsock 加载错误" << std::endl;
	}
	//std::cout << "加载 Winsock 成功" << std::endl;
}


CInitSock::~CInitSock() {
	WSACleanup();
	//std::cout << "清理 Winsock 完毕" << std::endl;
}
