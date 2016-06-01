// Komenda do kompilacji:   g++ main.cpp -lmp3lame -o PCM_to_MP3		
// Uruchomienie:            ./PCM_to_MP3 <nazwa_pliku_konwertowanego>.pcm <nazwa_pliku>.mp3		
 

#include <stdio.h>
#include <iostream>
#include <lame/lame.h>
#include <stdlib.h>
#include "basic_functions.h"
#define KONIEC 1
#define SIZE 30


// Rezygnujemy z podawania w konsoli nazwy plikow docelowych
// Menu bedzie zaprojektowane aby podawanie plikow bylo efektywniejsze

int main ()  
{
    int temp;
    int wybor;
    char first[SIZE];
    char second[SIZE];		

    while (KONIEC)
    {
    	Menu ();
    	scanf ("%d", &wybor);
   

    switch(wybor)
       {
    case 1 :	fgets(first, SIZE, stdin);
	        fgets(second, SIZE, stdin);
	        read_save(first, second);
	        break;
    case 2 :    exit(EXIT_SUCCESS);  // zakonczenie dzialania programu w tym momencie
                break;

    default :   printf ("Spróbuj ponownie \n");
                while (getchar() != '\n');
                break;

}


    
}


