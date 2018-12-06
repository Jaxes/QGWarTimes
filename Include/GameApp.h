#ifndef GAMEAPP_H
#define GAMEAPP_H

class Director;
class Sence;
class Sprite2D;
class Sprite3D;

#include <WS2tcpip.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <DirectXColors.h>
#include <RenderStates.h>
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>
#include <DirectXCollision.h>
#include <fstream>
#include <sstream>

#include "D3DApp.h"
#include "LightHelper.h"
#include "Camera.h"
#include "Director.h"
#include "Sence.h"
#include "Sprite2D.h"
#include "Sprite3D.h"
#include "Animator.h"
#include "ConstantBuffer.h"

#include "Client.h"
#include "ClientInterface.h"
#include "InitSock.h"

class GameApp : public D3DApp
{
public:
	// �����ģʽ
	enum class CameraMode { FirstPerson, ThirdPerson, Free };
	
public:
	GameApp(HINSTANCE hInstance);
	~GameApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	// objFileNameInOutΪ����õ���ɫ���������ļ�(.*so)������ָ��������Ѱ�Ҹ��ļ�����ȡ
	// hlslFileNameΪ��ɫ�����룬��δ�ҵ���ɫ���������ļ��������ɫ������
	// ����ɹ�����ָ����objFileNameInOut���򱣴����õ���ɫ����������Ϣ�����ļ�
	// ppBlobOut�����ɫ����������Ϣ
	HRESULT CreateShaderFromFile(const WCHAR* objFileNameInOut, const WCHAR* hlslFileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** ppBlobOut);

	std::shared_ptr<WarTimes::CWarTimes> pGame;             //��Ϸ��Ϣ
	std::shared_ptr<Director> mDirector;                    //��Ϸ�ܵ���
private:
	bool InitEffect();
	bool InitResource();

	bool InitFightSence();
	bool InitStartSence();
	bool InitModeSelectSence();
	bool InitRoomSelectSence();
	bool InitPositionSelectSence();
	bool InitWaitOthersSence();
	bool InitLoadingSence();
	bool InitSettlementSence();
	bool InitClient();
private:
	
	ComPtr<ID2D1SolidColorBrush> mColorBrush1;				// ��ɫ��ˢ1
	ComPtr<ID2D1SolidColorBrush> mColorBrush2;				// ��ɫ��ˢ2
	ComPtr<IDWriteFont> mFont;								// ����
	ComPtr<IDWriteTextFormat> mTextFormat;					// �ı���ʽ

	ComPtr<ID3D11InputLayout> mVertexLayout2D;				// ����2D�Ķ������벼��
	ComPtr<ID3D11InputLayout> mVertexLayout3D;				// ����3D�Ķ������벼��
	ComPtr<ID3D11InputLayout> mVertexLayout2DColor;         // ����2DColor�Ķ������벼��
	ComPtr<ID3D11Buffer> mConstantBuffers[5];				// ����������

	ComPtr<ID3D11VertexShader> mVertexShader3D;				// ����3D�Ķ�����ɫ��
	ComPtr<ID3D11PixelShader> mPixelShader3D;				// ����3D��������ɫ��
	ComPtr<ID3D11VertexShader> mVertexShader2D;				// ����2D�Ķ�����ɫ��
	ComPtr<ID3D11PixelShader> mPixelShader2D;				// ����2D��������ɫ��
	ComPtr<ID3D11VertexShader> mVertexShader2DColor;        // ���������ڸ����2D������ɫ��
	ComPtr<ID3D11PixelShader> mPixelShader2DColor;          // ���������ڸ����2D������ɫ��

	CBChangesEveryFrame mCBFrame;							// �û�������Ž���ÿһ֡���и��µı���
	CBChangesOnResize mCBOnReSize;							// �û�������Ž��ڴ��ڴ�С�仯ʱ���µı���
	CBNeverChange mCBNeverChange;							// �û�������Ų����ٽ����޸ĵı���
	CBMaterial mMaterial;                                   // ��������

	ComPtr<ID3D11SamplerState> mSamplerState;				// ������״̬

	std::shared_ptr<Camera> mCamera;						// �����
	CameraMode mCameraMode;									// �����ģʽ

	bool isStart;
	bool isReady;

	std::shared_ptr<Client> mClient;

	std::wstring Name;

	std::shared_ptr<ClientInterface> mClientInterface;

	std::shared_ptr<Animator<DirectX::XMFLOAT2>> Anim;

	std::shared_ptr<Sprite3D> GameMap;
	std::shared_ptr<Sprite3D> Man;
	std::shared_ptr<Sprite3D> Gun;
	std::shared_ptr<Sprite3D> Player[4];
	std::shared_ptr<Sprite3D> mWalls[4];
	std::shared_ptr<Sprite3D> SubmachineGun[4];
	//std::shared_ptr<Sprite3D> ShotGun[4];
	std::shared_ptr<Sprite2D> Start[4];
	std::shared_ptr<Sprite2D> modeSelect[13];
	std::shared_ptr<Sprite2D> roomSelect[6];
	std::shared_ptr<Sprite2D> posSelect[13];
	std::shared_ptr<Sprite2D> WaitOthers[22];
	std::shared_ptr<Sprite2D> UI[28];
	std::shared_ptr<Sprite2D> Loading[2];
	std::shared_ptr<Sprite2D> Settlement[5];
};


#endif