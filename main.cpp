// Komenda do kompilacji:   g++ <nazwa_pliku.cpp> -lmp3lame -o PCM_to_MP3
// Uruchomienie:            ./PCM_to_MP3 <nazwa_pliku_konwertowanego>.pcm <nazwa_pliku>.mp3

#include <stdio.h>
#include <iostream>
#include <lame/lame.h>

//Dodalem argumenty wywolania programu, zeby nie trzeba bylo ich recznie zmieniac.
int main(int argc, char **argv)
{
    int read, write;

    FILE *pcm = fopen("file.pcm", "rb");

    //Sprawdzam czy konwertowany plik .pcm istnieje, zeby nie naruszac pamieci gdy sie pomylimy przy wpisywaniu.
	if(pcm==0)
	{
		std::cout<<"Plik "<<argv[1]<<" nie istnieje!!!"<<std::endl;
		return -1;
	}

    //Plik .mp3 jest tworzony automatycznie, jesli nie istnieje, wiec nie ma potrzeby sprawdzania.
    FILE *mp3 = fopen("file.mp3", "wb");

    const int PCM_SIZE = 8192;
    const int MP3_SIZE = 8192;

    short int pcm_buffer[PCM_SIZE*2];
    unsigned char mp3_buffer[MP3_SIZE];

    lame_t lame = lame_init();
    lame_set_in_samplerate(lame, 44100);
    lame_set_VBR(lame, vbr_default);
    lame_init_params(lame);

    do {
        read = fread(pcm_buffer, 2*sizeof(short int), PCM_SIZE, pcm);
        if (read == 0)
            write = lame_encode_flush(lame, mp3_buffer, MP3_SIZE);
        else
            write = lame_encode_buffer_interleaved(lame, pcm_buffer, read, mp3_buffer, MP3_SIZE);
        fwrite(mp3_buffer, write, 1, mp3);
    } while (read != 0);

    lame_close(lame);
    fclose(mp3);
    fclose(pcm);

    return 0;
}
