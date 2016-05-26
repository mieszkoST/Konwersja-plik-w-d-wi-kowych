#include <stdio.h>
#include <iostream>
#include <lame/lame.h>
#define KONIEC 1


pcm_to_mp3(char *, char*);



int main(int argc, int ** argv)
{
    int temp;
    int wybor;

    while(KONIEC)
    {
    Menu();
    scanf("%d", &wybor);
    }
}
    switch(wybor)
    {
        case 1 :       File *mp3 =fopen("file.mp3", "wb");    /* wb zamazuje stara tresc, otwiera plik w trybie binarnym*/
                       File *pcm = fopen("file.pcm", "rb");   /* rb otwiera plik do czytania, otwiera plik w trybie bianarnym*/
                       if (pcm == NULL)
                       {
                           return NULL;
                       }
                       else
                       {
                       temp = pcm_to_mp3(char *pcm, char *mp3);
                       }
                       break;

        case 2 :       return NULL;
                       break;

        default :      printf("Spróbuj ponownie \n");
                       while(getchar() != '\n'');
                       break;


    }
}


void Menu()
{
    printf("To jest konwerter audio PCM na MP3.\n");
    printf("Czy chcesz go uzyc ?");
    printf("1 - Tak, chce przekonwertowac swój plik PCM na MP3");
    printf("2 - Nie, nie chce konwertowac");
}
