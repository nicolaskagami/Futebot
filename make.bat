set LIB=".\SoccerPlayer_Library"
g++ %LIB%\Futebot.cpp %LIB%\sock.cpp %LIB%\environm.cpp -o Futebot.exe -Wno-write-strings -lws2_32 -lm
