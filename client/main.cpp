#define UNICODE
#define _UNICODE
#include <iostream>
#include <windows.h>
#include <string>
#include <tchar.h>

#include "SmartHandle.h"
#include "DataObject.h"
#include "debug_module.h"

const std::basic_string<TCHAR> PIPE_NAME = TEXT("MyPipe");
const uint16_t BUFSIZE = 512;

int _tmain(VOID) {
    //LPCTSTR fullPipeName = (TEXT("\\\\.\\pipe\\") + PIPE_NAME).c_str();
    std::basic_string<TCHAR> fullPipeName = TEXT("\\\\.\\pipe\\") + PIPE_NAME;
    _tprintf(TEXT("full pipe name: %s\n"), fullPipeName.c_str());

    return 0;
}
