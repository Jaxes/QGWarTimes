#ifndef DIRECTOR_H
#define DIRECTOR_H

enum class SENCE;

#include <memory>
#include <WinSock2.h>
#include "wterr.h"
#include "Sence.h"
#include "RenderStates.h"

class Director
{
public:
	// ʹ��ģ�����(C++11)��������
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	Director(ComPtr<ID3D11DeviceContext> immediateContext);
	~Director();
	WTRESULT AddSence(std::shared_ptr<Sence> sence);
	WTRESULT DirectorDrawSence();
	WTRESULT SetSence(SENCE sence);
	SENCE GetSence();
private:
	std::vector<std::shared_ptr<Sence>> mDirector;
	// D3D11�豸������
	ComPtr<ID3D11DeviceContext> md3dImmediateContext;
	SENCE sence;
};

#endif //DIRECTOR_H
