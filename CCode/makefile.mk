output: VirtualHanc.o
	g++ VirtualHanc.o -o output

VirtualHanc.o: VirtualHanc.c
	g++ -c VirtualHanc.c