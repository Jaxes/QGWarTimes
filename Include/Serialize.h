#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <iostream>
#include <memory>
#include <sstream>
#include <map>
#include <vector>
#include <functional>
#include <cstdint>

#include <WinSock2.h>
#include <Windows.h>

// 序列化接口
// 需要序列化的类可实现接口进行序列化
class ISerialize {
public:
	// 应当实现的工厂方法, 构造空对象
	//static std::shared_ptr<ISerialize> construct();
	// 从数据流中还原对象, 限空对象调用, 要求数据需为同一个类用 toString 生成的数据流
	virtual std::wistream& fromString(std::wistream& in) = 0;
	// 将对象序列化输出到数据流中, 要求一行一个数据
	virtual std::wostream& toString(std::wostream& out) const = 0;

	// 根据对象名获取工厂函数
	static std::map<std::wstring, std::function<std::shared_ptr<ISerialize>()>> constructors;
	// 将可序列化对象序列化并输出到流中
	static bool encode(std::shared_ptr<ISerialize> const in, std::wostream& out);
	// 从流中提取对象, 要求对象的工厂函数已经注册到 ISerialize::constructors 映射表中
	static bool decode(std::wistream& in, std::shared_ptr<ISerialize>& out);

	static bool encode(std::vector<std::shared_ptr<ISerialize>> const& in, std::wostream& out);
	static bool decode(std::wistream& in, std::vector<std::shared_ptr<ISerialize>>& out);
};

// 宽字符转多字节字符
std::wstring toWString(std::string str);

// 序列化助手 帮助可序列化类自动向序列化注册工厂函数
// BUG注意: 要求 ISerialize::constructors 的初始化要比序列化助手的静态变量的初始化先进行
template<class T>
class SerializeHelper {
public:
	SerializeHelper() {
		ISerialize::constructors[toWString(typeid(T).name())] = T::construct;
	}
};

// 如果是 Serialize.cpp 包含此文件, 则令其全局变量初始化优先级设为 lib 级
// 否则设为 user 级, 让 Serialize 的全局变量比其他全局变量要先创建
#ifdef SERIALIZE_CPP
#pragma init_seg(lib)
#else
#pragma init_seg(user)
#endif

#endif
