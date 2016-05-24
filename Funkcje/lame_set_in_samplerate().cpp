//Nie znalaz³em pe³nego cia³¹ ten funkcji. Wyczyta³em jednak, ¿e s³u¿y ona do ustawiania czêstotliwoœci próbkowania.
//Uwaga: Jeœli prêdkoœæ ta nie zostanie okreœlona przez u¿ytkownika, jest ustawiana najwy¿sza mo¿liwa, czyli 44,1 kHz.
//Jak wam siê uda znaleŸæ pe³n¹ funkcjê, to zaktualizujcie ten plik ;-)

public static int lame_set_in_samplerate(lame_global_flags flags, int rate)
input sample rate in Hz. default = 44100hz
int CDECL lame_set_in_samplerate(lame_global_flags , int);
