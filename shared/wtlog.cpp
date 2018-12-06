#include "wtlog.h"

int WTLOG_DYNAMIC_FILTER = WTLOG_ERROR;

void(*pWTLogOutput)(const wchar_t*) = OutputDebugString;

int WTLog(int _In_ __level, const wchar_t* _In_ format, ...) {
	if(__level < WTLOG_DYNAMIC_FILTER) {
		return 0;
	}
	static wchar_t buf[WTLOG_MAX_DEFAULT_OUTPUT_BUFFER_SIZE];
	va_list list;
	va_start(list, format);
	int hr = wvsprintf(buf, format, list);
	va_end(list);
	pWTLogOutput(buf);
	return hr;
}
