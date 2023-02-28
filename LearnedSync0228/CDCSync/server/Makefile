server: server.o
	gcc -o server.run server.o -lcrypto

%.o: %.c chunkList.h fastcdc.h
	gcc -c -g $< -o $@

clean:
	rm -rf *.o

