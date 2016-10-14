
TEST_SOURCES = test1.c test2.cs
SOURCES = pe32-loader.c
TEST_TARGETS = test32.exe test64.exe clr32.exe clr64.exe
TARGETS = pe32-loader

all: $(TEST_TARGETS) $(TARGETS)


pe32-loader: pe32-loader.c
	gcc -O2 -o pe32-loader pe32-loader.c
	strip pe32-loader


test32.exe: test1.c
	x86_64-w64-mingw32-gcc -m32 -o test32.exe test1.c
	x86_64-w64-mingw32-strip test32.exe


test64.exe: test1.c
	x86_64-w64-mingw32-gcc -o test64.exe test1.c
	x86_64-w64-mingw32-strip test64.exe


clr32.exe: test2.cs
	mcs -out:clr32.exe -t:exe test2.cs


clr64.exe: test2.cs
	mcs -platform:x64 -out:clr64.exe -t:exe test2.cs


clean: $(TEST_TARGETS) $(TARGETS)
	rm -fv $(TEST_TARGETS) $(TARGETS)



