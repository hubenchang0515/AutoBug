.PHONY: all install clean

all: AutoBug Accelerator.cpp 

install: all


clean: 
	rm -f DataLoader.o DimMap.o main.o Kmeans.o Text.o


AutoBug : DataLoader.o DimMap.o main.o Kmeans.o Text.o Accelerator.cpp 
	g++ -o $@ $^ `pkg-config --libs OpenCL` 

DataLoader.o: DataLoader.cpp DataLoader.h DimMap.h Text.h
	g++ -c  DataLoader.cpp -O2 -W -Wall `pkg-config --cflags OpenCL` 

DimMap.o: DimMap.cpp DimMap.h
	g++ -c  DimMap.cpp -O2 -W -Wall `pkg-config --cflags OpenCL` 

main.o: main.cpp DimMap.h Text.h DataLoader.h Kmeans.h Accelerator.h
	g++ -c  main.cpp -O2 -W -Wall `pkg-config --cflags OpenCL` 

Kmeans.o: Kmeans.cpp Kmeans.h Text.h DimMap.h Accelerator.h
	g++ -c  Kmeans.cpp -O2 -W -Wall `pkg-config --cflags OpenCL` 

Text.o: Text.cpp Text.h DimMap.h
	g++ -c  Text.cpp -O2 -W -Wall `pkg-config --cflags OpenCL` 

Accelerator.cpp :  Accelerator.cxx kernel.cl prepare.sh 
	bash -c ./prepare.sh 

