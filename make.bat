set SOCCER_LIB=".\SoccerPlayer_Library"
set NEURAL=".\Neural"
set SRC=".\src"
set BIN=".\bin"
g++ %SRC%\Futebot.cpp %SOCCER_LIB%\sock.cpp %SOCCER_LIB%\environm.cpp -o %BIN%\Futebot_Fuzzy.exe -Wno-write-strings -lws2_32 -lm
g++ %SRC%\principal.cpp %NEURAL%\stlfn.c  %SOCCER_LIB%\sock.cpp %SOCCER_LIB%\environm.cpp -o %BIN%\Futebot_Neural.exe -Wno-write-strings -lws2_32 -lm
g++ %SRC%\tlfn.c -o %BIN%\tlfn.exe -O3 -Wno-write-strings -lws2_32 -lm
