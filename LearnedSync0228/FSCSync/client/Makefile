host: client.o
	gcc -o client.run client.o -lcrypto

%.o: %.c chunkList.h fastcdc.h hashmap.h match.h 
	gcc -c -g $< -o $@

clean:
	rm -rf *.o

