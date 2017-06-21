all: lightshot_direct.exe Makefile
	killprocess.bat Lightshot.exe &
	
analyze.dll: analyze.cpp Makefile
	g++ -c -DBUILDING_ANALYZE_DLL analyze.cpp
	g++ -shared -o analyze.dll analyze.o -lcurldll
	rm analyze.o
	
lightshot_direct.exe: analyze.dll lightshot_direct.au3 lightshot.ico Makefile
	killprocess.bat lightshot_direct.exe & 
	autoc //in lightshot_direct.au3 //out lightshot_direct.exe //icon lightshot.ico
