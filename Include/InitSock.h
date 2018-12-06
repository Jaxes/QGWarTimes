#ifndef INITSOCK_H
#define INITSOCK_H

#include <iostream>

#include <winsock2.h>
#include <Windows.h>

#pragma comment(lib, "WS2_32.lib")

class CInitSock {
public:
	CInitSock(BYTE minorVer = 2, BYTE majorVer = 2);
	~CInitSock();
};

#endif //INITSOCK_H