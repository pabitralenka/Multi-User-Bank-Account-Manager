compile : 
	gcc -o server server.c -lm -lpthread
	gcc -o client client.c


runserver :
	./server 8888


runclient individually :
	./client.o 127.0.0.1 8888 0.2 Transactions.txt


runclient simultaneously :
  	chmod 755 script.sh
  	./script.sh

  
clean :
	rm server client
