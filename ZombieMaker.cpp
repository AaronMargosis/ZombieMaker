// ZombieMaker.cpp : Program that creates zombie process and thread objects for demonstration/testing purposes.
//

#include <windows.h>
#include <iostream>
#include <sstream>
#include <conio.h>
#include "StringUtils.h"
#include "Utilities.h"
#include "SysErrorMessage.h"

//TODO: Test getting a single zombie process or thread handle and then duplicating it thousands of times

void Syntax(const wchar_t* argv0)
{
	std::wstring sExe = GetFileNameFromFilePath(argv0);
	std::wcerr
		<< L"Syntax:" << std::endl
		<< std::endl
		<< L"  To create zombie processes:" << std::endl
		<< L"    " << sExe << L" [-n:count] [-p] [-t] [-m:milliseconds] [-j]" << std::endl
		<< std::endl
		<< L"  To leak threads in this process:" << std::endl
		<< L"    " << sExe << L" [-n:count] [-T | -TZ]" << std::endl
		<< std::endl
		<< L"  -n  : specify number of processes or threads to start (default 10)" << std::endl
		<< L"  -p  : don't leak process handles" << std::endl
		<< L"  -t  : don't leak thread handles returned by CreateProcess" << std::endl
		<< L"  -m  : wait specified number of milliseconds between each CreateProcess (default 0)" << std::endl
		<< L"  -j  : assign processes to an unnamed job object" << std::endl
		<< L"  -T  : create [count] threads that hang and do not exit within this process and leak those handles" << std::endl
		<< L"  -TZ : create [count] zombie threads within this process and leak those handles" << std::endl
		<< std::endl;
	exit(-1);
}

/// <summary>
/// Thread that does nothing but exit.
/// Supports the -TZ command line option.
/// </summary>
DWORD WINAPI NopThread(LPVOID)
{
	return 0;
}

/// <summary>
/// Thread that does nothing but hang.
/// Supports the -T command line option.
/// </summary>
DWORD WINAPI HungThread(LPVOID)
{
	Sleep(INFINITE);
	return 0;
}

// Program that creates zombie process and thread objects for demonstration/testing purposes.
int wmain(int argc, wchar_t** argv)
{
	int numProcessesOrThreads = 10;
	DWORD dwMilliseconds = 0;
	bool bLeakProcessHandles = true, bLeakThreadHandles = true;
	bool bAssignToJob = false;
	bool bLeakThreadsInThisProcess = false, bZombieThreadsInThisProcess = false;

	for (int ixCurrArg = 1; ixCurrArg < argc; ++ixCurrArg)
	{
		const wchar_t* szCurrArg = argv[ixCurrArg];
		if (L'-' != szCurrArg[0])
			Syntax(argv[0]);
		switch (szCurrArg[1])
		{
		case L'n':
			if (L':' != szCurrArg[2])
				Syntax(argv[0]);
			if (1 != swscanf_s(&szCurrArg[3], L"%d", &numProcessesOrThreads))
				Syntax(argv[0]);
			if (numProcessesOrThreads <= 0)
				Syntax(argv[0]);
			break;
		case L'p':
			bLeakProcessHandles = false;
			break;
		case L't':
			bLeakThreadHandles = false;
			break;
		case L'm':
			if (1 != swscanf_s(&szCurrArg[3], L"%u", &dwMilliseconds))
				Syntax(argv[0]);
			break;
		case L'j':
			bAssignToJob = true;
			break;
		case L'T':
			bLeakThreadsInThisProcess = true;
			if (L'Z' == szCurrArg[2])
				bZombieThreadsInThisProcess = true;
			break;
		default:
			Syntax(argv[0]);
		}
	}

	HANDLE hJob = nullptr;
	if (bAssignToJob)
	{
		hJob = CreateJobObjectW(nullptr, nullptr); // TODO: add an option to make a randomly-named job object instead of unnamed
		if (nullptr != hJob)
		{
			JOBOBJECT_EXTENDED_LIMIT_INFORMATION jobInfo = { 0 };
			jobInfo.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
			if (!SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &jobInfo, sizeof(jobInfo) ))
			{
				DWORD dwLastErr = GetLastError();
				std::wcerr << L"SetInformationJobObject failed: " << SysErrorMessageWithCode(dwLastErr) << std::endl;
				return -2;
			}
		}
		else
		{
			DWORD dwLastErr = GetLastError();
			std::wcerr << L"CreateJobObjectW failed: " << SysErrorMessageWithCode(dwLastErr) << std::endl;
			return -2;
		}
	}

	int ix = 0;
	if (!bLeakThreadsInThisProcess)
	{
		// ZombieProc[32].exe should be in the same directory as this executable.
		// 64-bit child process name
		const wchar_t* szZombieProc = L"\\ZombieProc.exe";
#pragma warning(push)
#pragma warning(disable:4127) // "conditional expression is constant"
		if (4 == sizeof(void*))
#pragma warning(pop)
		{
			// 32-bit child process name
			szZombieProc = L"\\ZombieProc32.exe";
		}

		std::wstring sZombieProcPath = ThisExeDirectory() + szZombieProc;

		for (ix = 0; ix < numProcessesOrThreads; ++ix)
		{
			if (0 == (ix + 1) % 100)
			{
				// Write progress to the console with CR but no LF to overwrite previous lines
				std::wcout << L"Progress: " << (ix + 1) << L" ...          \r" << std::flush;
			}
			STARTUPINFOW startupInfo = { 0 };
			startupInfo.cb = sizeof(startupInfo);
			PROCESS_INFORMATION pi = { 0 };
			const DWORD dwCreationFlags = CREATE_BREAKAWAY_FROM_JOB | CREATE_NEW_PROCESS_GROUP;
			BOOL ret = CreateProcessW(sZombieProcPath.c_str(), nullptr, nullptr, nullptr, FALSE, dwCreationFlags, nullptr, nullptr, &startupInfo, &pi);
			if (ret)
			{
				if (bAssignToJob)
				{
					if (!AssignProcessToJobObject(hJob, pi.hProcess))
					{
						DWORD dwLastErr = GetLastError();
						std::wcerr << L"AssignProcessToJobObject failed: " << SysErrorMessageWithCode(dwLastErr) << std::endl;
					}
				}
				if (!bLeakProcessHandles)
					CloseHandle(pi.hProcess);
				if (!bLeakThreadHandles)
					CloseHandle(pi.hThread);
				if (0 != dwMilliseconds)
					Sleep(dwMilliseconds);
			}
			else
			{
				DWORD dwLastErr = GetLastError();
				std::wcout << L"CreateProcessW failed: " << SysErrorMessageWithCode(dwLastErr) << std::endl;
				break;
			}
		}
		size_t nHandlesLeaked = 0;
		if (bLeakProcessHandles)
			nHandlesLeaked += ix;
		if (bLeakThreadHandles)
			nHandlesLeaked += ix;
		std::wcout
			<< std::endl
			<< L"Processes started: " << ix << std::endl
			<< L"Leaked handles:    " << nHandlesLeaked << std::endl
			<< std::endl;
	}
	else
	{
		for (ix = 0; ix < numProcessesOrThreads; ++ix)
		{
			HANDLE hLeakMe = CreateThread(NULL, 0, (bZombieThreadsInThisProcess ? NopThread : HungThread), NULL, 0, NULL);
			if (NULL == hLeakMe)
			{
				DWORD dwLastErr = GetLastError();
				std::wcout << L"CreateThread failed: " << SysErrorMessageWithCode(dwLastErr) << std::endl;
				break;
			}
		}
		std::wcout
			<< std::endl
			<< (bZombieThreadsInThisProcess ? L"Zombie threads" : L"Threads") <<  L" leaked: " << ix << std::endl
			<< std::endl;
	}
	std::wcout << L"Press any key to exit and to release handles ";
// Suppress warning about ignored return value from _getch()
#pragma warning(suppress: 6031)
	_getch();
	return 0;
}

