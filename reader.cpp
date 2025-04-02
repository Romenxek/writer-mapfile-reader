#include <windows.h>
#include <iostream>
#include <conio.h>
#include <tchar.h>

int main() {
    TCHAR memName[] = TEXT("SharedMemory");
    
    HANDLE hMapFileCreatingEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, TEXT("MappingEvent"));
    if (!hMapFileCreatingEvent) 
    {
        hMapFileCreatingEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("MappingEvent"));
        if (!hMapFileCreatingEvent)
        {
            std::cerr << "MapFileCreatingEvent in reader failed: " << GetLastError() << std::endl;
            std::cin.get();
            return 1;
        }
    }

    HANDLE hMapFile = OpenFileMapping(FILE_MAP_READ,FALSE,memName);
    if (!hMapFile) 
    {
        hMapFile = CreateFileMapping(
            INVALID_HANDLE_VALUE, //дескриптор
            NULL,                 //атрибуты безопасности
            PAGE_READWRITE,       //режим доступа
            0,                    //размер отображени€ (0 - весь файл)
            256,                  //высший значащий байт размера файла
            memName               //им€ отображени€ файла 
        );
        if (!hMapFile) {
            std::cerr << "CreateFileMapping in reader failed: " << GetLastError() << std::endl;
            std::cin.get();
            return 1;
        }
        SetEvent(hMapFileCreatingEvent);
    }
    WaitForSingleObject(hMapFileCreatingEvent, INFINITE); 

    char* pBuf = (char*)MapViewOfFile(
        hMapFile,
        FILE_MAP_READ,
        0,
        0,
        256
    );
    if (!pBuf) {
        std::cerr << "MapViewOfFile in reader failed: " << GetLastError() << std::endl;
        CloseHandle(hMapFile);
        std::cin.get();
        return 1;
    }

    HANDLE hMessageReadyEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, TEXT("MessageReady"));
    if (!hMessageReadyEvent) {
        hMessageReadyEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("MessageReady"));
        if (!hMessageReadyEvent) {
            std::cerr << "CreateEvent failed: " << GetLastError() << std::endl;
            UnmapViewOfFile(pBuf);
            CloseHandle(hMapFile);
            return 1;
        }
    }

    HANDLE hReaderIsFinished = OpenEvent(EVENT_ALL_ACCESS, FALSE, TEXT("ReaderIsReading"));
    if (!hReaderIsFinished) {
        hReaderIsFinished = CreateEvent(NULL, FALSE, TRUE, TEXT("ReaderIsReading"));
        if (!hReaderIsFinished) {
            std::cerr << "CreateEvent failed: " << GetLastError() << std::endl;
            UnmapViewOfFile(pBuf);
            CloseHandle(hMapFile);
            return 1;
        }
    }

    while (true)
    {
        WaitForSingleObject(hMessageReadyEvent, INFINITE);
        ResetEvent(hReaderIsFinished);
        std::cout << "Message is reading" << std::endl;
        /// ¬ѕё—ј“№ ¬–≈ћя —ёƒј  |
        ///                     |
        ///                     |
        ///                     V
                        Sleep(2000);
        std::cout << "Received message: " << pBuf << std::endl;
        ResetEvent(hMessageReadyEvent);
        SetEvent(hReaderIsFinished);
    }

    CloseHandle(hReaderIsFinished);
    CloseHandle(hMessageReadyEvent);
    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);
    CloseHandle(hMapFileCreatingEvent);

    return 0;
}