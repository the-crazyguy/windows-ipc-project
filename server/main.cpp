#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <string>
#include <thread>

const std::string PIPE_NAME = "MyPipe";
const uint16_t BUFSIZE = 512;

struct Data {
    int id{0};
    double value{0};
};

DWORD WINAPI old_HandleClientThread(LPVOID paramPtr) {
    //old
}

void ProcessClientThread(HANDLE pipeHandle) {

}

int _tmain(VOID) {
    LPCTSTR fullPipeName = TEXT(("\\\\.\\pipe\\" + PIPE_NAME).c_str());

    //TODO: Add a way to (properly) stop the program via ctrl-c or the like
    while (true)
    {
        HANDLE pipeHandle = CreateNamedPipe(
        fullPipeName, //name
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
            // Create a thread to handle the client
            //eww, windows api
            /*
            DWORD threadId = 0;
            HANDLE threadHandle = CreateThread(
                nullptr, //no security attribute
                0,  //default stack size
                HandleClientThread, //thread procedure
                static_cast<LPVOID>(pipeHandle),    //thread parameter
                0,  //not suspended
                &threadId   //returns thread id
            );

            if (threadHandle == nullptr) {
                std::cerr << "CreateThread failed: " << GetLastError() << std::endl;

                return -1; 
            }
            else {
                //Thread created, close the handle
                CloseHandle(threadHandle);
            }
            */
           
            std::thread processingThread(ProcessClientThread, pipeHandle);
            processingThread.detach();  //run independently
        }
        else {
            std::cout << "Client could not connect." << std::endl;
            CloseHandle(pipeHandle);
        }
    }
    
    return 0;
}
