// Komenda do kompilacji:   g++ <nazwa_pliku.cpp> -lmp3lame -o PCM_to_MP3
// Uruchomienie:            ./PCM_to_MP3 <nazwa_pliku_konwertowanego>.pcm <nazwa_pliku>.mp3

#include <stdio.h>
#include <iostream>
#include <lame/lame.h>
#include "basic_functions.h"

//Dodalem argumenty wywolania programu, zeby nie trzeba bylo ich recznie zmieniac.

int main(int argc, char **argv)
{
    int status = 1;
    int choice = 0;
    int temp;

    while (status)
    {
        system("clear");
        Menu();
        scanf("%d", &choice);

        switch(choice)
        {
            case 1 :        FILE *mp3 = fopen("file.mp3", "wb");
                            FILE *pcm = fopen("file.pcm", "rb");
                            if (pcm == NULL) exit(FAILED);
                            temp = pcm_to_mp3(char * pcm, char * mp3);
                            break;

            case 2 :        exit(EXIT_SUCCESS);
                            break;

            default :       printf("Try again \n");
                            while(getchar() != '\n');
                            break;
    }

    return 0;
}
