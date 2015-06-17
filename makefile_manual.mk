# mingan:
# don't forget to load the compiler:
#       module load gcc/4.9.2
# you will also need to change the GSL variable below to the commented version

CC=gcc

GSL=-lgsl -lgslcblas
# for mingan, comment out the above and uncomment the line below
#GSL=-Wl,-Bstatic -lgsl -lgslcblas -Wl,-Bdynamic

CF=-std=c9x -iquote includes

all: build/stmodel

build/stmodel: build/main.o build/grid.o build/climate.o
	$(CC) $(CF) -o build/stmodel build/main.o build/grid.o build/climate.o $(GSL)
	
build/main.o: src/main.c includes/grid.h includes/climate.h
	$(CC) $(CF) -c -o build/main.o src/main.c
	
build/grid.o: src/grid.c includes/grid.h
	$(CC) $(CF) -c -o build/grid.o src/grid.c

build/climate.o: src/climate.c includes/climate.h
	$(CC) $(CF) -c -o build/climate.o src/climate.c