/*
文件提供以下宏, 函数, 枚举值进行错误处理

宏: WTRESULT
宏: ASSERT_ELSE
宏: ASSERT_RETURN
宏: WTR
函数: WTStrErr
以及一系列 WTRESULT_ 开头的, 具有错误值的枚举常量, 详见 WTRESULT_ENUM_STRS 宏定义处
另外可自行添加自定义的错误枚举值

宏 WTRESULT
即 int, 用于表示 WarTimes 游戏专用错误值
函数返回 WTRESULT 型错误码表示程序运行情况, 该错误码
若为 0 则表示程序运行一切正常
若为 -1 则表示未知错误(某种意义上讲是所有异常的基类)
若为其他值, 则可以用 WTStrErr 函数获取错误值对应的错误字符串

例:
WTWARN(L"%s", WTStrErr(WTRESULT_OK));

开启 WARN 级日志后可以看到输出 "一切正常"

宏 ASSERT_ELSE(表达式, 错误提示值):
断定表达式一定成立, 否则将错误表达式和错误提示值写入错误日志
因为 Release 下该语句会被优化到不存在, 所以不宜在该语句内做自增自减赋值等操作

例 一个打开文件的操作:
std::ifstream infile;
infile.open("tmp.txt", std::ios::in);
ASSERT_ELSE(infile.fail() == false, 0);
// 断定文件打开一定成功, 如果不成功则记录日志, 但因为没有什么提示错误的信息, 所以只能随便用个 0 作为错误提示值

例 一次运算:
int a = 1, b = 3;
int c = a + b;
ASSERT_ELSE(c == a + b, c);
// 断定 c 一定等于 a + b, 如果 c 不等于 a + b, 则记录日志, 同时也记录 c 的值, 以方便调试查看 c 的值发生了怎样的"变异"

宏 ASSERT_RETURN(表达式, 错误值):
仅限返回值为 WTRESULT 型的函数使用
断定表达式一定成立, 否则将错误表达式和错误值对应的字符串写入错误日志, 并返回错误值, 表示函数失败
Release 下不记录日志, 但是仍然会判断表达式是否成立, 为零则返回错误值

例 一个打开文件的操作:
std::ifstream infile;
infile.open("tmp.txt", std::ios::in);
ASSERT_RETURN(infile.fail() == false, RESULT_FileOpenFailed);
// 断定文件打开一定成功, 如果不成功则记录日志, 记录日志时, 会自动将 RESULT_FileOpenFailed 转成对应的字符串 L"文件打开失败" 后再做日志

宏 WTR(错误值):
表示用默认方式处理错误值
Release 下只计算其值, 不做处理

函数 WTStrErr(错误值):
获取错误值对应的字符串常量

错误枚举值:
以 RESULT_ 开头的一系列标识符
WTRESULT_ENUM_STRS 定义了一系列的标识符的后缀与字符串的映射关系

例:
WTRESULT_ENUM_STR(枚举标识符后缀, 对应字符串)

将前缀 RESULT_ 和后缀连在一起即可组成枚举标识符, 其值即为错误值
使用 WTStrErr 可由错误值得到对应的字符串

例:
WTStrErr(RESULT_FileOpenFailed)
可得到字符串 L"文件打开失败"

可在其后仿照已有格式新增自定义的错误值

*/

#ifndef WTERR_H
#define WTERR_H

// 错误值类型
#ifndef WTRESULT
#define WTRESULT int
#endif

// 可去掉, 去掉则ASSERT检查时不做日志
#include "wtlog.h"

// ASSERT_ELSE 断定表达式一定成立, 否则写错误日志
#if defined(DEBUG) || defined(_DEBUG)
#ifdef WTLOG_H
// 断定表达式一定成立, 否则写错误日志
#define ASSERT_ELSE(__exp, __errval) \
	do{ \
		if(!(__exp)) WTERROR(L"%s: %d", L#__exp, (int)(__errval)); \
	}while(0)
#else
// 断定表达式一定成立, 否则abort
#define ASSERT_ELSE(__exp, __errval) \
	do{ \
		if(!(__exp)) abort(); \
	}while(0)
#endif
#else
// 断定表达式一定成立, 否则写错误日志, 当前选项下(非DEBUG)该断言不做日志
#define ASSERT_ELSE(__exp, __errval)
#endif

// ASSERT_RETURN 检查表达式是否成立, 不成立则发出错误, 返回错误码, 并记录日志
#if defined(DEBUG) || defined(_DEBUG)
#ifdef WTLOG_H
// 检查表达式是否成立, 不成立则发出错误, 返回错误码, 并记录日志
#define ASSERT_RETURN(__exp, __errcode) \
	do { \
		bool __res = (__exp); \
		if(!__res) { \
			WTRESULT __errcode_tmp = __errcode; \
			WTERROR("ASSERT_RETURN: %s 不成立, %s", L#__exp, WTStrErr(__errcode_tmp)); \
			return (__errcode_tmp); \
		} \
	} while(0)
#else
// 检查表达式是否成立, 不成立则发出错误, 并返回错误码
#define ASSERT_RETURN(__exp, __errcode) \
	do { if(!(__exp)) { return (__errcode); } while(0)
#endif
#else
// 检查表达式是否成立, 不成立则发出错误, 并返回错误码
#define ASSERT_RETURN(__exp, __errcode) \
	do if(!(__exp)) { return (__errcode); } while(0)
#endif

// WTR 用默认方式处理错误
#if defined(DEBUG) || defined(_DEBUG)
// 用默认方式处理错误
#define WTR(__errcode) \
	do { \
		WTRESULT __val = (__errcode); \
		if(__val) { \
			WTFATAL("WTR 收到错误: %s", WTStrErr(__val)); \
			abort(); \
		} \
	} while(0)
// ps: 不必考虑是否include了日志模块
#else
// 用默认方式处理错误, 该选项下默认不处理任何结果
#define WTR(__errcode) (__errcode)
#endif

// 通过错误码获取对应的错误字符串
const wchar_t* WTStrErr(WTRESULT ErrorNo);

// 为将错误码定义和字符串靠得尽量仅, 使用XMARCO
// 在此处添加错误枚举值和字符串
// WTRESULT_ENUM_STR 的第一个参数输入枚举值后缀, 前缀为 WTRESULT_ , 第二个参数输入错误值对应的字符串
#define WTRESULT_ENUM_STRS \
	WTRESULT_ENUM_STR( OK                      ,  L"一切正常"             ) \
	WTRESULT_ENUM_STR( UserNameError           ,  L"用户名错误"           ) \
	WTRESULT_ENUM_STR( HwndIsNull              ,  L"HWND为空"             ) \
	WTRESULT_ENUM_STR( IntervalTooSmall        ,  L"间隔时间过短"         ) \
	WTRESULT_ENUM_STR( TimerCreateFailed       ,  L"定时器创建失败"       ) \
	WTRESULT_ENUM_STR( TimerIsRunning          ,  L"定时器已经启动"       ) \
	WTRESULT_ENUM_STR( TimerIsNotRunning       ,  L"定时器未启动"         ) \
	WTRESULT_ENUM_STR( TimerKillFailed         ,  L"定时器销毁失败"       ) \
	WTRESULT_ENUM_STR( CallbackFunctionIsNull  ,  L"回调函数为空"         ) \
	WTRESULT_ENUM_STR( AnimeIsEnd              ,  L"动画已经结束"         ) \
	WTRESULT_ENUM_STR( FileOpenFailed          ,  L"文件打开失败"         ) \
	WTRESULT_ENUM_STR( TextureAddFailed        ,  L"纹理添加失败"         ) \
	WTRESULT_ENUM_STR( ConnectingTimeOut       ,  L"连接超时"             ) \
	WTRESULT_ENUM_STR( CreateSocketError       ,  L"创建套接字失败"       ) \
	WTRESULT_ENUM_STR( EventSettingError       ,  L"事件设置失败"         ) \
	WTRESULT_ENUM_STR( ClientError             ,  L"客户端异常"           ) \
	WTRESULT_ENUM_STR( ClientIsRunning         ,  L"客户端已在运行"       ) \
	WTRESULT_ENUM_STR( ServiceIsRunning        ,  L"服务端已在运行"       ) \
	WTRESULT_ENUM_STR( SendFailed              ,  L"发送失败"             ) \
    WTRESULT_ENUM_STR( NotASpriteFromFile      ,  L"这不是一个从文件读取的精灵")

enum _WTRESULT {
#define WTRESULT_ENUM_STR(__e, __s) WTRESULT_##__e,
	WTRESULT_ENUM_STRS
#undef WTRESULT_ENUM_STR
	WTRESULT_All
};

#endif
