## Description:
Catch phantom processes on the system

## Remarks:
After some tinkering in C#, using some external libraries for getting events from the windows even tracing (ETW), and having some annoying problems with it, I've decided to go with the C++ route
to see if I can make the whole thing without it being unnecessarily slow and bloated.

## Build:
```
cl ..\src\phantom.cpp user32.lib Gdi32.lib Comctl32.lib
```
This will be updated as we go.