ashell: ashell.o
	g++ -Wall -o ashell ashell.o

ashell.o: aShell.cpp
	g++ -Wall -c aShell.cpp

clean:
	rm -f ashell.o ashell