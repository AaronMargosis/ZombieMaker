#include <Windows.h>
#include "StringUtils.h"
#include "Utilities.h"



/// <summary>
/// Returns the path to the directory in which the current executable image is.
/// </summary>
const std::wstring& ThisExeDirectory()
{
	// Initialize sThisExeDirectory the first time it's called in this process:
	static std::wstring sThisExeDirectory;
	if (0 == sThisExeDirectory.length())
	{
		wchar_t szPath[MAX_PATH + 1] = { 0 };
		if (GetModuleFileNameW(NULL, szPath, MAX_PATH))
		{
			sThisExeDirectory = GetDirectoryNameFromFilePath(szPath);
		}
	}
	return sThisExeDirectory;
}
