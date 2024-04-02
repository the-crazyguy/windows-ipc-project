#include <iostream>
#include <windows.h>
#include <string>

const std::string PIPE_NAME = "MyPipe";
const uint16_t BUFSIZE = 512;

struct Data {
    int id{0};
    double value{0};
};

int main(int, char**) {
    std::cout << "Hello, from ipc_server!\n";

    HANDLE pipeHandle;
    Data data;
    DWORD dataWritten;

    pipeHandle = CreateNamedPipe(
        ("\\\\.\\pipe\\" + PIPE_NAME).c_str(), //name
        PIPE_ACCESS_DUPLEX, // read/write
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,   //Max instances
        BUFSIZE,    //output buffer size
        BUFSIZE,    //input buffer size
        0,  //client timeout
        nullptr    //default security attributes
    );

    if (pipeHandle == INVALID_HANDLE_VALUE) {
        std::cerr << "CreateNamedPipe failed: " << GetLastError() << std::endl;

        return -1;    
    }

    bool connected = ConnectNamedPipe(pipeHandle, nullptr) ? 
        true : (GetLastError() == ERROR_PIPE_CONNECTED);    //true if connected successfully or already connected

    if (connected) {
        std::cout << "Client connected!" << std::endl;
    }
    else {
        std::cout << "Client could not connect." << std::endl;
        CloseHandle(pipeHandle);
    }
}
