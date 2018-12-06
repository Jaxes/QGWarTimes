#ifndef D3DAPP_H
#define D3DAPP_H

#include <string>

#include <WinSock2.h>
#include <Windows.h>
#include <d3d11.h>
#include <d2d1.h>
#include <dwrite.h>

#include <wrl/client.h> // ComPtr

#include <Mouse.h>
#include <Keyboard.h>

#include "dxerr.h"
#include "GameTimer.h"

// 移植过来的错误检查，该项目仅允许使用Unicode字符集, 用于处理 HRESULT 型返回值的错误
#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)																						\
do {																										\
	HRESULT hr = (x);																			\
	if(FAILED(hr)) {																			\
		DXTrace(__FILEW__, (DWORD)__LINE__, hr, L#x, true);	\
	}																											\
} while(0)

#endif
#else
#ifndef HR
#define HR(x) (x)
#endif
#endif 

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "winmm.lib")

class D3DApp {
public:
	// 在构造函数的初始化列表应当设置好初始参数
	D3DApp(HINSTANCE _In_ hInstance);
	virtual ~D3DApp();

	// 获取应用实例的句柄, 一个程序只能有一个
	HINSTANCE	AppInst()const;
	// 获取主窗口句柄, 由程序生成, 由程序管理
	HWND			MainWnd()const;
	float			AspectRatio()const;

	// 运行程序，执行消息事件的循环, 阻塞
	int Run();

	// 框架方法。客户派生类需要重载这些方法以实现特定的应用需求

	// 该父类方法需要初始化窗口, Direct2D和Direct3D部分
	virtual bool Init();
	// 该父类方法需要在窗口大小变动的时候调用
	virtual void OnResize();
	// 子类需要实现该方法，完成每一帧的更新
	virtual void UpdateScene(float dt) = 0;
	// 子类需要实现该方法，完成每一帧的绘制
	virtual void DrawScene() = 0;
	// 窗口的消息回调函数
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	// 窗口初始化
	bool InitMainWindow();
	// Direct2D初始化
	bool InitDirect2D();
	// Direct3D初始化
	bool InitDirect3D();

	// 计算每秒帧数并在窗口显示
	void CalculateFrameStats();

protected:
	// 应用实例句柄
	HINSTANCE	mhAppInst;
	// 主窗口句柄
	HWND			mhMainWnd;
	// 应用是否暂停
	bool			mAppPaused;
	// 应用是否最小化
	bool			mMinimized;
	// 应用是否最大化
	bool			mMaximized;
	// 窗口大小是否变化
	bool			mResizing;
	// MSAA支持的质量等级
	UINT			m4xMsaaQuality;

	// 计时器
	GameTimer mTimer;

	// 使用模板别名(C++11)简化类型名
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	// Direct2D
	// D2D工厂
	ComPtr<ID2D1Factory> md2dFactory;
	// D2D渲染目标
	ComPtr<ID2D1RenderTarget> md2dRenderTarget;
	// DWrite工厂
	ComPtr<IDWriteFactory> mdwriteFactory;

	// Direct3D 11
	// D3D11设备
	ComPtr<ID3D11Device> md3dDevice;
	// D3D11设备上下文
	ComPtr<ID3D11DeviceContext> md3dImmediateContext;
	// D3D11交换链
	ComPtr<IDXGISwapChain> mSwapChain;

	// 常用资源
	// 深度模板缓冲区
	ComPtr<ID3D11Texture2D> mDepthStencilBuffer;
	// 渲染目标视图
	ComPtr<ID3D11RenderTargetView> mRenderTargetView;
	// 深度模板视图
	ComPtr<ID3D11DepthStencilView> mDepthStencilView;
	// 视口
	D3D11_VIEWPORT mScreenViewport;

	// 键鼠输入
	// 鼠标
	std::unique_ptr<DirectX::Mouse> mMouse;
	// 鼠标状态追踪器
	DirectX::Mouse::ButtonStateTracker mMouseTracker;
	// 键盘
	std::unique_ptr<DirectX::Keyboard> mKeyboard;
	// 键盘状态追踪器
	DirectX::Keyboard::KeyboardStateTracker mKeyboardTracker;

	// 派生类应该在构造函数设置好这些自定义的初始参数
	// 主窗口标题
	std::wstring mMainWndCaption;
	// 视口宽度
	int mClientWidth;
	// 视口高度
	int mClientHeight;
};

#endif
