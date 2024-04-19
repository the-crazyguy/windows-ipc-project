#define UNICODE
#define _UNICODE
#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <string>
#include <thread>
#include <memory>
#include <exception>

#include "SmartHandle.h"
#include "DataObject.h"

const std::basic_string<TCHAR> PIPE_NAME = TEXT("MyPipe");
const uint16_t BUFSIZE = 512;

//Note: make it so it returns a success flag instead?

/// @brief Generates a response to the provided client request.
/// @param requestBuffer The client's request
/// @param responseBuffer A (preallocated) buffer to the server's response
/// @return The amount of bytes written in responseBuffer
DWORD GetResponseToClientRequest(const std::unique_ptr<TCHAR[]> &requestBuffer, 
                          std::unique_ptr<TCHAR[]> &responseBuffer) {
    std::basic_string<TCHAR> request(requestBuffer.get());

    //Note: handle reply based on request
    _tprintf(TEXT("Client request: %s\n"), request.c_str());

    std::basic_string<TCHAR> genericResponse = TEXT("OK");

    DWORD responseBytes = 0;
    HRESULT copyResult = StringCchCopy(responseBuffer.get(), BUFSIZE, genericResponse.c_str());
    
    if (SUCCEEDED(copyResult)) {
        responseBytes = (lstrlen(responseBuffer.get()) + 1) * sizeof(TCHAR);
    }
    else {
        std::cerr << "GetResponseToClientRequest failed to copy server's response into the provided buffer 'response'" << std::endl;
        
        responseBytes = 0;
        responseBuffer[0] = 0;
    }

    return responseBytes;
}

void ProcessClientThread(HANDLE pipeHandle) {
    if (pipeHandle == nullptr) {
        std::cerr << "pipeHandle got unexepcted nullptr value" << std::endl;

        return;
    }

    //Memory allocation
    std::unique_ptr<TCHAR[]> clientRequest;
    std::unique_ptr<TCHAR[]> serverResponse;

    try
    {
        clientRequest.reset(new TCHAR[BUFSIZE]);
        serverResponse.reset(new TCHAR[BUFSIZE]);
    }
    catch(const std::bad_alloc& e)
    {
        std::cerr << "Memory allocation to process a client failed: " << 
            e.what() << std::endl;
        return;
    }
    

    // Loop until done reading
    while (true)
    {
        DWORD bytesRead = 0;
        bool successfulRead = ReadFile(
            pipeHandle,
            clientRequest.get(),    //receive data buffer (raw)
            BUFSIZE,    
            &bytesRead,
            nullptr //not overlapped I/O
        );

        //!!! BREAK !!!
        if (!successfulRead || bytesRead == 0) {
            if (GetLastError() == ERROR_BROKEN_PIPE) {
                std::cout << "ProcessClientThread: Client disconnected." << std::endl;
            }
            else {
                std::cout << "ProcessClientThread: ReadFile failed, last error: " << GetLastError() << std::endl;
            }

            break;
        }

        DWORD responseBytes = GetResponseToClientRequest(clientRequest, serverResponse);
        DWORD writtenBytes = 0;
        bool successfulWrite = WriteFile(
            pipeHandle,
            serverResponse.get(),
            responseBytes,
            &writtenBytes,
            nullptr // not overlapped I/O
        );

        //!!! BREAK !!!
        if (!successfulWrite || (responseBytes != writtenBytes)) {
            std::cerr << "ProcessClientThread failed to write response, last error: " << GetLastError() << std::endl;

            break;
        }
    }

    //Flush and disconnect
    FlushFileBuffers(pipeHandle);
    DisconnectNamedPipe(pipeHandle);
    CloseHandle(pipeHandle);    //Note: implement SmartHandle

    std::cout << "ProcessClientThread exiting." << std::endl;
}

int _tmain(VOID) {
    //LPCTSTR fullPipeName = (TEXT("\\\\.\\pipe\\") + PIPE_NAME).c_str();
    std::basic_string<TCHAR> fullPipeName = TEXT("\\\\.\\pipe\\") + PIPE_NAME;


    //TODO: Add a way to (properly) stop the program via ctrl-c or the like
    while (true)
    {
        HANDLE pipeHandle = CreateNamedPipe(
        fullPipeName.c_str(), //name
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
