//Nie znalaz�em pe�nego cia�� ten funkcji. Wyczyta�em jednak, �e s�u�y ona do ustawiania cz�stotliwo�ci pr�bkowania.
//Uwaga: Je�li pr�dko�� ta nie zostanie okre�lona przez u�ytkownika, jest ustawiana najwy�sza mo�liwa, czyli 44,1 kHz.
//Jak wam si� uda znale�� pe�n� funkcj�, to zaktualizujcie ten plik ;-)

public static int lame_set_in_samplerate(lame_global_flags flags, int rate)
input sample rate in Hz. default = 44100hz
int CDECL lame_set_in_samplerate(lame_global_flags , int);
