# Makefile for radiopenet.ro metadata grabber
# LIBS: libcurl, libmysqlclient, rapidjson, libconfig

CC=g++
CFLAGS=-std=c++20 -I/usr/local/include -I/usr/include/mysql -I /usr/include/curl
LDFLAGS=-L/usr/lib -L/usr/lib64 -L/usr/lib64/mysql -lconfig++ -lmysqlclient -lcurl 

main: main.cpp
	$(CC) $(CFLAGS) main.cpp src/*.cpp $(LDFLAGS) -o main