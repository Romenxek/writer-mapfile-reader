#include <windows.h>
#include <iostream>
#include <conio.h>
#include <tchar.h>

int main() {

    STARTUPINFO SI;
    ZeroMemory(&SI, sizeof(SI));
    PROCESS_INFORMATION readerPI, writerPI;

    TCHAR readerName[] = TEXT("reader.exe");
    TCHAR writerName[] = TEXT("writer.exe");

    if (!CreateProcess(
        writerName,
        NULL,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &SI,
        &writerPI
    ))
    {
        std::cerr << "Process writer.exe didn't created: " << GetLastError() << std::endl;
        return 1;
    }

    if (!CreateProcess(
        readerName,
        NULL,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &SI,
        &readerPI
    ))
    {
        std::cerr << "Process reader.exe didn't created: " << GetLastError() << std::endl;
        return 1;
    }

    WaitForSingleObject(writerPI.hProcess, INFINITE);
    WaitForSingleObject(readerPI.hProcess, INFINITE);

    std::cin.get();

    CloseHandle(readerPI.hProcess);
    CloseHandle(readerPI.hThread);
    CloseHandle(writerPI.hProcess);
    CloseHandle(writerPI.hThread);
    return 0;
}
//‘айл, отображаемый в пам€ть (memory-mapped file, MMF) Ч это механизм, который позвол€ет отобразить содержимое файла 
//в адресное пространство процесса, так что доступ к данным файла осуществл€етс€ так же, как к обычной пам€ти.