#include "GameApp.h"

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE prevInstance,
	PSTR cmdLine,
	int showCmd) {

	// 不使用的参数
	UNREFERENCED_PARAMETER(prevInstance);
	UNREFERENCED_PARAMETER(cmdLine);
	UNREFERENCED_PARAMETER(showCmd);

	// 允许在DEBUG版本进行运行时内存分配和泄漏检测
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	
	WTLOG_DYNAMIC_FILTER = WTLOG_DEBUG;
	
	WTINFO("游戏启动了");

	GameApp theApp(hInstance);

	WTINFO("游戏创建了");

	if(!theApp.Init())
		return 0;

	WTINFO("游戏初始化了, 准备运行");

	return theApp.Run();
}
