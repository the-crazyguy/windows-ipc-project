#define UNICODE
#define _UNICODE
#include <iostream>
#include <windows.h>

const std::basic_string<TCHAR> PIPE_NAME = TEXT("MyPipe");
const uint16_t BUFSIZE = 512;

int main(int, char**) {
    std::cout << "Hello, from ipc_client!\n";
}
