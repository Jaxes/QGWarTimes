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
	// 摄像机模式
	enum class CameraMode { FirstPerson, ThirdPerson, Free };
	
public:
	GameApp(HINSTANCE hInstance);
	~GameApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	// objFileNameInOut为编译好的着色器二进制文件(.*so)，若有指定则优先寻找该文件并读取
	// hlslFileName为着色器代码，若未找到着色器二进制文件则编译着色器代码
	// 编译成功后，若指定了objFileNameInOut，则保存编译好的着色器二进制信息到该文件
	// ppBlobOut输出着色器二进制信息
	HRESULT CreateShaderFromFile(const WCHAR* objFileNameInOut, const WCHAR* hlslFileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob** ppBlobOut);

	std::shared_ptr<WarTimes::CWarTimes> pGame;             //游戏信息
	std::shared_ptr<Director> mDirector;                    //游戏总导演
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
	
	ComPtr<ID2D1SolidColorBrush> mColorBrush1;				// 单色笔刷1
	ComPtr<ID2D1SolidColorBrush> mColorBrush2;				// 单色笔刷2
	ComPtr<IDWriteFont> mFont;								// 字体
	ComPtr<IDWriteTextFormat> mTextFormat;					// 文本格式

	ComPtr<ID3D11InputLayout> mVertexLayout2D;				// 用于2D的顶点输入布局
	ComPtr<ID3D11InputLayout> mVertexLayout3D;				// 用于3D的顶点输入布局
	ComPtr<ID3D11InputLayout> mVertexLayout2DColor;         // 用于2DColor的顶点输入布局
	ComPtr<ID3D11Buffer> mConstantBuffers[5];				// 常量缓冲区

	ComPtr<ID3D11VertexShader> mVertexShader3D;				// 用于3D的顶点着色器
	ComPtr<ID3D11PixelShader> mPixelShader3D;				// 用于3D的像素着色器
	ComPtr<ID3D11VertexShader> mVertexShader2D;				// 用于2D的顶点着色器
	ComPtr<ID3D11PixelShader> mPixelShader2D;				// 用于2D的像素着色器
	ComPtr<ID3D11VertexShader> mVertexShader2DColor;        // 用于生成遮盖面的2D顶点着色器
	ComPtr<ID3D11PixelShader> mPixelShader2DColor;          // 用于生成遮盖面的2D像素着色器

	CBChangesEveryFrame mCBFrame;							// 该缓冲区存放仅在每一帧进行更新的变量
	CBChangesOnResize mCBOnReSize;							// 该缓冲区存放仅在窗口大小变化时更新的变量
	CBNeverChange mCBNeverChange;							// 该缓冲区存放不会再进行修改的变量
	CBMaterial mMaterial;                                   // 材质属性

	ComPtr<ID3D11SamplerState> mSamplerState;				// 采样器状态

	std::shared_ptr<Camera> mCamera;						// 摄像机
	CameraMode mCameraMode;									// 摄像机模式

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