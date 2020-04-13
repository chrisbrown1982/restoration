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

pth.3: ./pth.3.cpp
	g++ pth.3.cpp -lpthread -o pth.3

runpth.3: ./pth.3
	./pth.3 && mv ./results ./results_pth.3.txt

pth.4: ./pth.4.cpp
	g++ pth.4.cpp -lpthread -o pth.4

runpth.4: ./pth.4
	./pth.4 && mv ./results ./results_pth.4.txt

pth.4.alt: ./pth.4.alt.cpp
	g++ pth.4.alt.cpp -lpthread -o pth.4.alt

runpth.4.alt: ./pth.4.alt
	./pth.4.alt && mv ./results ./results_pth.4.alt.txt

pth.5.alt: ./pth.5.alt.cpp
	g++ pth.5.alt.cpp -lpthread -o pth.5.alt

runpth.5.alt: ./pth.5.alt
	./pth.5.alt && mv ./results ./results_pth.5.alt.txt