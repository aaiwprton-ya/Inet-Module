CFLAGS := -Wall -g -std=c++20
CLIBFLAGS := -c
server_args := --addr=0.0.0.0 --port=8000
client_args := --addr=127.0.0.1 --port=8000
libs := lib

inet_module.a: server.o client.o socket.o
	ar rcs $@ $^

server.o: server.cpp server.h | socket.o std_afx.h
	g++ $(CLIBFLAGS) $(CFLAGS) -c -o $@ $< socket.o

client.o: client.cpp client.h | socket.o std_afx.h
	g++ $(CLIBFLAGS) $(CFLAGS) -o $@ $< socket.o

socket.o: socket.cpp socket.h | std_afx.h
	g++ $(CLIBFLAGS) $(CFLAGS) -o $@ $<
	
test_server.out: test_server.cpp
	g++ $(CFLAGS) -o $@ $^ inet_module.a $(libs)/argparser.a

test_client.out: test_client.cpp
	g++ $(CFLAGS) -o $@ $^ inet_module.a $(libs)/argparser.a

.PHONY: server client debug_server debug_client clean

server: test_server.out
	./test_server.out $(server_args)

client: test_client.out
	./test_client.out $(client_args)

debug_server: test_server.out
	gdb --args ./test_server.out $(server_args)

debug_client: test_client.out
	gdb --args ./test_client.out $(client_args)

clean:
	rm *.o *.a *.out

