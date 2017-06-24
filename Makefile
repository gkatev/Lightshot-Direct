all: lightshot_direct.exe

lightshot_direct.exe: lightshot_direct.au3 Makefile
	autoc //in lightshot_direct.au3 //out lightshot_direct.exe
