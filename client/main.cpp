#define UNICODE
#define _UNICODE
#include <iostream>
#include <windows.h>
#include <string>
#include <tchar.h>
#include <conio.h>

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

        //!!! BREAK !!!
        if ( pipeHandle != INVALID_HANDLE_VALUE ) {
            break;
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
    std::basic_string<TCHAR> messageToSend = TEXT("HelloWorld!");
    DWORD bytesToWrite = ( lstrlen( messageToSend.c_str() ) + 1 ) * sizeof(TCHAR);  // use this instead of lstrlen? static_cast<DWORD>(messageToSend.size())
    DWORD bytesWritten = 0;

    bool isSendSuccessful = WriteFile(
        pipeHandle,
        messageToSend.c_str(),
        bytesToWrite,
        &bytesWritten,
        nullptr //not overlapped 
    );

    if ( !isSendSuccessful ) {
        std::cerr << "WriteFile to pipe server failed. Last error: " <<
                     GetLastError() <<
                     std::endl;
        return -1;
    }

    Debug::LogA("Message sent to server.");
    
    bool isDoneReading{false};
    bool readSuccess{false};
    std::basic_string<TCHAR> fullServerResponse;

    do {
        DWORD bytesRead{0};
        TCHAR serverResponse[BUFSIZE];

        readSuccess = ReadFile(
            pipeHandle,
            serverResponse,
            BUFSIZE * sizeof(TCHAR),
            &bytesRead,
            nullptr //not overlapped
        );
        if ( readSuccess ) {
            isDoneReading = true;
            fullServerResponse += serverResponse;
        } else if ( GetLastError() == ERROR_MORE_DATA ) {
            fullServerResponse += serverResponse;
        } else {
            //Unhandled error, stop reading
            isDoneReading = true;
        }

    } while ( !isDoneReading );
    
    if ( fullServerResponse.size() > 0 ) {
        _tprintf(TEXT("Server response: %s\n"), fullServerResponse.c_str());
    }

    if ( !readSuccess ) {
        std::cerr << "ReadFile from pipe server failed. Last error: " <<
                     GetLastError() <<
                     std::endl;
        return -1;
    }

    std::wcout << "End of client logic, press any key to terminate connection to server and exit..." << std::endl;
    _getch();

    CloseHandle(pipeHandle);

    return 0;
}
