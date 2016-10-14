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

Have fun
:-)
