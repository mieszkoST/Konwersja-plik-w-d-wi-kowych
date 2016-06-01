#include "basic_functions.h"


void Menu()
{
    printf("To jest konwerter audio PCM na MP3.\n");
    printf("Czy chcesz go uzyc ?");
    printf("1 - Tak, chce przekonwertowac swój plik PCM na MP3");
    printf("2 - Nie, nie chce konwertowac");
}

void read_save(char * read, char * save)
{	
	        FILE *mp3 = fopen(save, "wb");    /* wb zamazuje stara tresc, otwiera plik w trybie binarnym*/
                FILE *pcm = fopen(read, "rb");   /* rb otwiera plik do czytania, otwiera plik w trybie bianarnym*/
                if (pcm == NULL)
                {
                    exit(EXIT_FAILURE);
                }
                else
                {
                   int temp = pcm_to_mp3(pcm, mp3);
                }
                break;

