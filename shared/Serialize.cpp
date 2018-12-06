#define SERIALIZE_CPP
#include "Serialize.h"

std::wstring toWString(std::string str) {
	int size = MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.size(), 0, 0);
	if(size <= 0) {
		return L"";
	}
	std::wstring wstr;
	wstr.resize(size + 1);
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.size(), &wstr[0], wstr.size());
	wstr.back() = L'\0';
	if(wstr.front() == 0xFEFF) {
		wstr.erase(wstr.front());
	}
	return std::move(wstr);
}

std::map<std::wstring, std::function<std::shared_ptr<ISerialize>()>> ISerialize::constructors = {};

bool ISerialize::encode(std::shared_ptr<ISerialize> const in, std::wostream & out) {
	out << toWString(typeid(*in).name()) << std::endl;
	in->toString(out);
	return true;
}

bool ISerialize::decode(std::wistream & in, std::shared_ptr<ISerialize>& out) {
	std::wstring name;
	getline(in, name);
	out = nullptr;
	if(name.size() > 0) {
		auto constructor = ISerialize::constructors.find(name);
		if(constructor != ISerialize::constructors.end()) {
			out = constructor->second();
			out->fromString(in);
			return true;
		} else {
			std::wcerr << L"未知类: " << name << std::endl;
			return false;
		}
	}
	return false;
}

bool ISerialize::encode(std::vector<std::shared_ptr<ISerialize>> const& in, std::wostream& out) {
	for(auto const& i : in)
		ISerialize::encode(i, out);
	return true;
}

bool ISerialize::decode(std::wistream& in, std::vector<std::shared_ptr<ISerialize>>& out) {
	while(in.good()) {
		std::shared_ptr<ISerialize> obj;
		if(ISerialize::decode(in, obj) == true && obj != nullptr)
			out.push_back(obj);
	}
	return true;
}
