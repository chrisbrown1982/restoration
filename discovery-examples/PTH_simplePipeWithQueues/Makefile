# CC=g++
CC=clang++

pth: ./original.c
	$(CC) original.c -lpthread -o pth

runpth: ./pth
	time ./pth && mv ./results ./results_pth.txt

pth.1: ./pth.1.cpp
	$(CC) pth.1.cpp -lpthread -o pth.1

runpth.1: ./pth.1
	time ./pth.1 && mv ./results ./results_pth.1.txt

pth.2: ./pth.2.cpp
	$(CC) pth.2.cpp -lpthread -o pth.2

runpth.2: ./pth.2
	time ./pth.2 && mv ./results ./results_pth.2.txt

pth.3: ./pth.3.cpp
	$(CC) pth.3.cpp -lpthread -o pth.3

runpth.3: ./pth.3
	time ./pth.3 && mv ./results ./results_pth.3.txt

pth.4: ./pth.4.cpp
	$(CC) pth.4.cpp -lpthread -o pth.4

runpth.4: ./pth.4
	time ./pth.4 && mv ./results ./results_pth.4.txt

pth.5: ./pth.5.cpp
	$(CC) pth.5.cpp -lpthread -o pth.5

runpth.5: ./pth.5
	time ./pth.5 && mv ./results ./results_pth.5.txt

pth.6: ./pth.6.cpp
	$(CC) pth.6.cpp -lpthread -o pth.6

runpth.6: ./pth.6
	time ./pth.6 && mv ./results ./results_pth.6.txt

pth.7: ./pth.7.cpp
	$(CC) pth.7.cpp -lpthread -o pth.7

runpth.7: ./pth.7
	time ./pth.7 && mv ./results ./results_pth.7.txt

pth.8: ./pth.8.cpp
	$(CC) pth.8.cpp -lpthread -o pth.8

runpth.8: ./pth.8
	time ./pth.8 && mv ./results ./results_pth.8.txt

pth.9: ./pth.9.cpp
	$(CC) pth.9.cpp -lpthread -ltbb -o pth.9

runpth.9: ./pth.9
	time ./pth.9 && mv ./results ./results_pth.9.txt

pth.tbb: ./pth.tbb.cpp
	$(CC) pth.tbb.cpp -lpthread -ltbb -o pth.tbb

runpth.tbb: ./pth.tbb
	time ./pth.tbb && mv ./results ./results_pth.tbb.txt

forloops : ./forloops.c
	$(CC) forloops.c -lpthread -o forloops

forloops.1 : ./forloops.1.cpp
	$(CC) forloops.1.cpp -o forloops.1

forloops.2 : ./forloops.2.cpp
	$(CC) forloops.2.cpp -o forloops.2

forloops.3 : ./forloops.3.cpp
	$(CC) forloops.3.cpp -o forloops.3

forloops.4 : ./forloops.4.cpp
	$(CC) forloops.4.cpp -o forloops.4

forloops.5 : ./forloops.5.cpp
	$(CC) forloops.5.cpp -o forloops.5

forloops.6 : ./forloops.6.cpp
	$(CC) forloops.6.cpp -o forloops.6

forloops.7 : ./forloops.7.cpp
	$(CC) forloops.7.cpp -o forloops.7

breakloops : ./breakloops.cpp
	$(CC) breakloops.cpp -lpthread -o breakloops