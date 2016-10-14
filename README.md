# pe32-loader
It's a Linux utility to filter a PE32 *.exe and execute the image using either Wine 32/64 or Mono/Core CLR respectively

I know we can execute an .EXE on Linux using binfmt_misc, and it works.
But it does not have a way to know the differences between Mono/CLR binaries and Native .EXE

So we always have an option to use either:

:CLR:M::MZ::/usr/bin/mono:

or

:WinEXE:M::MZ::/usr/bin/wine: 

or /usr/bin/wine64, or any path to wine binaries

I guess the most convenient way to execute any EXE is using a middle man like this one

---------------------------------------------------
To build the executables:
Just type 'make' and it will build all, including
test programs. (You will need x86_64-w64-mingw32-gcc in your PATH, a multilib one)

There are 4 test programs:
- test32.exe 
- test64.exe
- clr32.exe
- clr64.exe

They are just outputs your arguments to the program, nothing else,
but better then a common HelloWorld :-)

By the way, this program uses /opt/wine/bin/wine and wine64 respectively,
if you have Wine installation somewhere else, just edit the source code and change
the path accordingly, I don't want to complicate a simple task by adding autoconf/automake
into this tree.


Have fun
:-)
