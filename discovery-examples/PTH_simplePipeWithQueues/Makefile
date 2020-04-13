pth: ./original.c
	g++ original.c -lpthread -o pth

runpth: ./pth
	./pth && mv ./results ./results_pth.txt

pth.1: ./pth.1.c
	g++ pth.1.cpp -lpthread -o pth.1

runpth.1: ./pth.1
	./pth.1 && mv ./results ./results_pth.1.txt

pth.2: ./pth.2.cpp
	g++ pth.2.cpp -lpthread -o pth.2

runpth.2: ./pth.2
	./pth.2 && mv ./results ./results_pth.2.txt