#ifndef RENDERSTATES_H
#define RENDERSTATES_H

#include <WinSock2.h>
#include <d3d11_1.h>
#include <wrl/client.h>
#include "dxerr.h"

// ��ֲ�����Ĵ����飬����Ŀ������ʹ��Unicode�ַ���
#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)                                              \
{                                                          \
	HRESULT hr = (x);                                      \
	if(FAILED(hr))                                         \
	{                                                      \
		DXTrace(__FILEW__, (DWORD)__LINE__, hr, L#x, true);\
	}                                                      \
}
#endif

#else
#ifndef HR
#define HR(x) (x)
#endif
#endif 


class RenderStates
{
public:
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	static void InitAll(ComPtr<ID3D11Device> device);
	// ʹ��ComPtr�����ֹ��ͷ�

public:
	static ComPtr<ID3D11RasterizerState> RSWireframe;		// ��դ����״̬���߿�ģʽ
	static ComPtr<ID3D11RasterizerState> RSNoCull;			// ��դ����״̬���ޱ���ü�ģʽ
	static ComPtr<ID3D11RasterizerState> RSCullClockWise;	// ��դ����״̬��˳ʱ��ü�ģʽ

	static ComPtr<ID3D11SamplerState> SSLinearWrap;			// ������״̬�����Թ���
	static ComPtr<ID3D11SamplerState> SSAnistropicWrap;		// ������״̬���������Թ���

	static ComPtr<ID3D11BlendState> BSNoColorWrite;		// ���״̬����д����ɫ
	static ComPtr<ID3D11BlendState> BSTransparent;		// ���״̬��͸�����
	static ComPtr<ID3D11BlendState> BSAlphaToCoverage;	// ���״̬��Alpha-To-Coverage

	static ComPtr<ID3D11DepthStencilState> DSSMarkMirror;		// ���/ģ��״̬����Ǿ�������
	static ComPtr<ID3D11DepthStencilState> DSSDrawReflection;	// ���/ģ��״̬�����Ʒ�������
	static ComPtr<ID3D11DepthStencilState> DSSNoDoubleBlend;	// ���/ģ��״̬���޶��λ������
	static ComPtr<ID3D11DepthStencilState> DSSNoDepthTest;		// ���/ģ��״̬���ر���Ȳ���
	static ComPtr<ID3D11DepthStencilState> DSSNoDepthWrite;		// ���/ģ��״̬������Ȳ��ԣ���д�����ֵ
};



#endif
