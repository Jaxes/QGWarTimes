#include "wterr.h"

static const wchar_t* WTStrErrArr[] = {
#define WTRESULT_ENUM_STR(__e, __s) __s,
	WTRESULT_ENUM_STRS
#undef WTRESULT_ENUM_STR
	0
};

const wchar_t* WTStrErr(WTRESULT ErrorNo) {
	if(ErrorNo == -1) {
		return L"未知错误";
	}
	if(ErrorNo < WTRESULT_All) {
		return WTStrErrArr[ErrorNo];
	} else {
		return L"不正确的错误值";
	}
}
