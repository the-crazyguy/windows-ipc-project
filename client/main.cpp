#define UNICODE
#define _UNICODE
#include <iostream>
#include <windows.h>
#include <string>
#include <tchar.h>

#include "SmartHandle.h"
#include "DataObject.h"
#include "debug_module.h"

const std::basic_string<TCHAR> PIPE_NAME = TEXT( "MyPipe" );
const uint16_t BUFSIZE = 512;

#define SECONDS *1000
const DWORD PIPE_WAIT_TIMEOUT = 20 SECONDS;

int _tmain( VOID ) {
    std::basic_string<TCHAR> fullPipeName = TEXT( "\\\\.\\pipe\\" ) + PIPE_NAME;
    
    HANDLE pipeHandle = nullptr;
    //TODO: Add a way to (properly) stop the program via ctrl-c or the like
    while ( true ) {
        pipeHandle = CreateFile(
            fullPipeName.c_str(),   
            GENERIC_READ | GENERIC_WRITE, //r/w access
            0,  // no sharing
            nullptr,    //default security attributes
            OPEN_EXISTING,
            0,  //default attributes
            nullptr //no template file
        );

        if ( pipeHandle == INVALID_HANDLE_VALUE ) {
            std::cerr << "CreateFile failed: " << GetLastError() << std::endl;
            return -1;
        }

        if ( GetLastError() != ERROR_PIPE_BUSY ) {
            std::cerr << "Could not open pipe. Last error: " << GetLastError() << std::endl;
            return -1;
        }

        if ( !WaitNamedPipe(fullPipeName.c_str(), PIPE_WAIT_TIMEOUT) ) {
            std::cerr << "Could not open pipe after waiting for " << PIPE_WAIT_TIMEOUT << "ms." << std::endl;
            return -1;
        }
    }

    // Pipe is connected
    DWORD pipeReadMode = PIPE_READMODE_MESSAGE;
    bool isSetStateSucccessful = SetNamedPipeHandleState(
        pipeHandle,
        &pipeReadMode,  //new pipe mode
        nullptr,    //no max bytes
        nullptr // no max time
    );

    if ( !isSetStateSucccessful ) {
        std::cerr << "SetNamedPipeHandleState failed. Last error: " << GetLastError() << std::endl;
        return -1;
    }

    //Send simple message
    std::basic_string<TCHAR> messageToSend = TEXT("Hello world!");
    DWORD bytesWritten = 0;

    bool isSendSuccessful = WriteFile(
        pipeHandle,
        messageToSend.c_str(),
        static_cast<DWORD>(messageToSend.size()),
        &bytesWritten,
        nullptr //not overlapped 
    );

    return 0;
}
