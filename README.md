# ZombieMaker.exe and ZombieMaker32.exe

ZombieMaker is a test tool for creating zombie processes/threads for demonstration purposes (e.g., with ZombieFinder). 

ZombieMaker.exe is a command-line program that simply launches multiple instances of a "helper" program, ZombieProc.exe (ZombieProc32.exe 
for the 32-bit version), each time failing to close the returned process and thread handles. ZombieProc.exe is basically a "no-op"
program: all it does is to sleep for two seconds and then exit, never showing any UI.

The result will be zero instances of ZombieProc.exe running but kernel objects consuming kernel memory and representing the 
instances that had exited. Those kernel objects still exist because ZombieMaker.exe still holds handles to them.
ZombieMaker.exe prompts `Press any key to exit and to release handles`; when ZombieMaker.exe exits, all its resource handles are 
closed, and the kernel objects can then be released.

```
Syntax:

  For zombie processes:
    ZombieMaker.exe [-n:count] [-p] [-t] [-m:milliseconds] [-j]

  For leaked threads:
    ZombieMaker.exe [-n:count] [-T | -TZ]

  -n  : specify number of processes or threads to start (default 10)
  -p  : don't leak process handles
  -t  : don't leak thread handles returned by CreateProcess
  -m  : wait specified number of milliseconds between each CreateProcess (default 0)
  -j  : assign processes to an unnamed job object
  -T  : create [count] threads that hang and do not exit within this process and leak those handles
  -TZ : create [count] zombie threads within this process and leak those handles
```

When creating zombie processes, ZombieProc.exe/ZombieProc32.exe must be in the same directory with ZombieMaker.exe/ZombieMaker32.exe.
