# ZombieMaker.exe and ZombieMaker32.exe
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

When creating zombie processes, ZombieProc.exe/ZombieProc32.exe must be in the same directory.
