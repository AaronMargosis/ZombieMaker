// ZombieProc.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "ZombieProc.h"

// Windows GUI (non-console) process that simply exits two seconds after starting. Shows no UI.
int APIENTRY wWinMain(_In_ HINSTANCE, // hInstance,
                     _In_opt_ HINSTANCE, // hPrevInstance,
                     _In_ LPWSTR, //    lpCmdLine,
                     _In_ int) //       nCmdShow)
{
    //const size_t nAllocSize = 1024 * 1024 * 1024;
    //LPVOID pv = VirtualAlloc(nullptr, nAllocSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    //if (nullptr != pv)
    //{
    //    for (size_t ix = 0; ix < nAllocSize; ++ix)
    //    {
    //        ((unsigned char*)pv)[ix] = (unsigned char)(ix % 0xFF);
    //    }
    //}
    Sleep(2000);
    return 0;
}

