copy_buffer(gfc, mp3buffer, mp3buffer_size_remaining, 1);

Ta funkcja ma za zadanie kopiowanie informacji kt�re aktualnie znajduj� si� z buforze, celem ich dalszego u�ycia.

Argumenty to:
    1) lame_internal_flags *gfc;
    2) unsigned char *mp3buffer
    3) int mp3buffer_size_remaining
    4) Sta�a liczbowa, zapewne reprezentuje jak�� opcj� w funkcj�, ale dok�adnie nie wiem, nie znalaz�em cia�a funkcji.
