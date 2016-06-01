#include <stdio.h>
#include <lame/lame.h>



int pcm_to_mp3(char * pcm_, char * mp3_)
{
    int read, write;

    FILE *pcm = fopen (pcm_, "rb");

    //Sprawdzam czy konwertowany plik .pcm istnieje, zeby nie naruszac pamieci gdy sie pomylimy przy wpisywaniu.
	if (pcm == NULL)
		return -1;

    //Plik .mp3 jest tworzony automatycznie, jesli nie istnieje, wiec nie ma potrzeby sprawdzania.
    FILE *mp3 = fopen (mp3_, "wb");

    const int PCM_SIZE = 8192;
    const int MP3_SIZE = 8192;

    short int pcm_buffer [PCM_SIZE*2];
    unsigned char mp3_buffer [MP3_SIZE];

    lame_t lame = lame_init ();
    lame_set_in_samplerate (lame, 44100);
    lame_set_VBR (lame, vbr_default);
    lame_init_params (lame);

    do 
	{
        read = fread (pcm_buffer, 2*sizeof(short int), PCM_SIZE, pcm);
        if (read == 0)
            write = lame_encode_flush (lame, mp3_buffer, MP3_SIZE);
        else
            write = lame_encode_buffer_interleaved (lame, pcm_buffer, read, mp3_buffer, MP3_SIZE);
        fwrite (mp3_buffer, write, 1, mp3);
    } while (read != 0);

    lame_close (lame);
    fclose (mp3);
    fclose (pcm);

    return 0;
}
