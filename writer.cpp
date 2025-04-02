#include <windows.h>
#include <iostream>
#include <conio.h>
#include <tchar.h>

int main() {
    const char* message = "Soobshenie s pisatelya";
    TCHAR memName[] = TEXT("SharedMemory");

    HANDLE hMapFileCreatingEvent = OpenEvent(EVENT_MODIFY_STATE, FALSE, TEXT("MappingEvent"));
    if (!hMapFileCreatingEvent)
    {
        hMapFileCreatingEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("MappingEvent"));
        if (!hMapFileCreatingEvent)
        {
            std::cerr << "MapFileCreatingEvent in writer failed: " << GetLastError() << std::endl;
            std::cin.get();
            return 1;
        }
    }

    HANDLE hMapFile = OpenFileMapping(FILE_MAP_READ, FALSE, memName);
    if (!hMapFile)
    {
        hMapFile = CreateFileMapping( // НЕ ПЕРЕОПРЕДЕЛЯТЬ НИ В КОЕМ СЛУЧАЕ !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            INVALID_HANDLE_VALUE, //дескриптор
            NULL,                 //атрибуты безопасности
            PAGE_READWRITE,       //режим доступа
            0,                    //размер отображения (0 - весь файл)
            256,                  //высший значащий байт размера файла
            memName               //имя отображения файла 
        );
        if (!hMapFile) {
            std::cerr << "CreateFileMapping in writer failed: " << GetLastError() << std::endl;
            std::cin.get();
            return 1;
        }
        SetEvent(hMapFileCreatingEvent);
    }
    WaitForSingleObject(hMapFileCreatingEvent, INFINITE);

    char* pBuf = (char*)MapViewOfFile(
        hMapFile,           //дескриптор
        FILE_MAP_WRITE,     //тип доступа к объекту сопоставления
        0,                  //высокий порядок смещения файла
        0,                  //низкий порядок смещения
        256                 //количество байтов сопоставления с сопоставлением представления
    );
    if (!pBuf) {
        std::cerr << "MapViewOfFile in writer failed: " << GetLastError() << std::endl;
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
        WaitForSingleObject(hReaderIsFinished, INFINITE);
        strcpy_s(pBuf, 256, message);
        std::cout << "Message is writing\n";
        /// ВПЮСАТЬ ВРЕМЯ СЮДА  |
        ///                     |
        ///                     |
        ///                     V
                         Sleep(2000);
       std::cout << "Message sent\n";
       SetEvent(hMessageReadyEvent);
    }

    CloseHandle(hMessageReadyEvent);
    CloseHandle(hReaderIsFinished);
    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);
    CloseHandle(hMapFileCreatingEvent);
    return 0;
}

//https://it.vstu.by/courses/information_systems/system_programming/theory/memory_management/
//https://learn.microsoft.com/ru-ru/windows/win32/memory/creating-named-shared-memory