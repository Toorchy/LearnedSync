sc: server.o client.o 
	gcc -o client.run client.o
	gcc -o server.run server.o

test: ssl_server.o ssl_client.o
	gcc -Wall -o ssl_client.run ssl_client.o -lssl -lcrypto
	gcc -Wall -o ssl_server.run ssl_server.o -lssl -lcrypto

%.o: %.c
	gcc -c -g $< -o $@

clean:
	rm -rf *.o

