all : exec

main.o : main.c menu.h utils.h
	gcc -c main.c -o main.o

combattant.o : combattant.c combattant.h utils.h
	gcc -c combattant.c -o combattant.o
	
combat.o : combat.c combat.h combattant.h utils.h menu.h 
	gcc -c combat.c -o combat.o

menu.o : menu.c menu.h combattant.h combat.h utils.h
	gcc -c menu.c -o menu.o
        
utils.o : utils.c utils.h
	gcc -c utils.c -o utils.o
	

exec : combattant.o combat.o menu.o utils.o main.o
	gcc combattant.o combat.o menu.o utils.o main.o -o exec
	

