/*
可在项目任何包括了该头文件的位置使用

	WTDEBUG  除BUG时临时添加的输出
	WTINFO	 记录一般性信息
	WTWARN	 记录警告信息, 表示问题未按预期但是程序仍能正常运行
	WTERROR	 记录错误信息
	WTFATAL  记录最严重信息, 通常日志中最后一条信息会是 FATAL 级

	例子:

	WTINFO(L"Hello! %d", 123);

更改 WTLOG_STATIC_FILTER 或 WTLOG_DYNAMIC_FILTER 的值可以筛选掉低等级的日志

	其中 WTLOG_STATIC_FILTER 是编译前确定
	WTLOG_DYNAMIC_FILTER 可在运行时更改
	可取值:

	WTLOG_DEBUG
	WTLOG_INFO
	WTLOG_WARN
	WTLOG_ERROR
	WTLOG_FATAL

	例子:

	WTLOG_DYNAMIC_FILTER = WTLOG_ERROR;
	WTWARN(L"Hello!");
	WTLOG_DYNAMIC_FILTER = WTLOG_WARN;
	WTWARN(L"Hello!!");

	只有后面一个被记录

注意第一个参数必须是字符串字面量如:

	WTINFO(L"Hello");

	或

	const wchar_t* str = L"Hello";
	WTINFO(L"%s", str);

	不能是

	const wchar_t* str = L"Hello";
	WTINFO(str);

所有日志记录最终会调用 void(pWTLogOutput*)(const wchar_t*) 指针指向的函数进行输出
	指针默认指向会将字符串输出到 VS 即使窗口或输出窗口
	改变指针指向的函数可以自定义日志输出的地方

*/

#ifndef WTLOG_H
#define WTLOG_H
#include <cstdio>
#include <cstdarg>
#include <winsock.h>
#include <Windows.h>

// 日志等级
#define WTLOG_DEBUG 1
#define WTLOG_INFO  2
#define WTLOG_WARN  3
#define WTLOG_ERROR 4
#define WTLOG_FATAL 5
// #define WTLOG_CRITICAL 6 //不建议使用

// 日志筛选器 (编译期), 低于该级的日志不会被记录
#if defined(DEBUG) || defined(_DEBUG)
#define WTLOG_STATIC_FILTER WTLOG_DEBUG
#else
#define WTLOG_STATIC_FILTER WTLOG_INFO
#endif

// 日志筛选器 (运行期), 低于该级的日志不会被记录, 默认值为 WTLOG_STATIC_FILTER
extern int WTLOG_DYNAMIC_FILTER;

// 核心字符串输出函数指针
extern void(*pWTLogOutput)(const wchar_t*);

#define WTLOG_MAX_DEFAULT_OUTPUT_BUFFER_SIZE 1024 //WTLog使用缓冲区暂存日志字符串, 该宏表示缓冲区大小
int WTLog(int _In_ __level, const wchar_t* _In_ format, ...); // 核心日志函数
#define WTLOG_FORCE(__format, __level, ...) WTLog(__level, L"等级: %d, 文件: %s (行 %d) 函数 %s: " __format L"\n", __level, __FILEW__, (int)__LINE__, __FUNCTIONW__, __VA_ARGS__)

#if WTLOG_STATIC_FILTER > WTLOG_DEBUG
//除BUG时临时添加的输出, 但是当前设置下该宏函数没有效果
#define WTDEBUG(__format, ...)
#else
//除BUG时临时添加的输出
#define WTDEBUG(__format, ...) WTLOG_FORCE(__format, WTLOG_DEBUG, __VA_ARGS__)
#endif

#if WTLOG_STATIC_FILTER > WTLOG_INFO
//记录一般性信息, 但是当前设置下该宏函数没有效果
#define WTINFO(__format, ...)
#else
//记录一般性信息
#define WTINFO(__format, ...) WTLOG_FORCE(__format, WTLOG_INFO, __VA_ARGS__)
#endif

#if WTLOG_STATIC_FILTER > WTLOG_WARN
//记录警告信息, 表示问题未按预期但是程序仍能正常运行, 但是当前设置下该宏函数没有效果
#define WTWARN(__format, ...)
#else
//记录警告信息, 表示问题未按预期但是程序仍能正常运行
#define WTWARN(__format, ...) WTLOG_FORCE(__format, WTLOG_WARN, __VA_ARGS__)
#endif

#if WTLOG_STATIC_FILTER > WTLOG_ERROR
//记录错误信息, 但是当前设置下该宏函数没有效果
#define WTERROR(__format, ...)
#else
//记录错误信息
#define WTERROR(__format, ...) WTLOG_FORCE(__format, WTLOG_ERROR, __VA_ARGS__)
#endif

#if WTLOG_STATIC_FILTER > WTLOG_FATAL
//记录最严重信息, 通常日志中最后一条信息会是 FATAL 级, 但是当前设置下该宏函数没有效果
#define WTFATAL(__format, ...)
#else
//记录最严重信息, 通常日志中最后一条信息会是 FATAL 级
#define WTFATAL(__format, ...) WTLOG_FORCE(__format, WTLOG_FATAL, __VA_ARGS__)
#endif

#endif
