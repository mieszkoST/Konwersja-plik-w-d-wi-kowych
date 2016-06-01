copy_buffer(gfc, mp3buffer, mp3buffer_size_remaining, 1);

Ta funkcja ma za zadanie kopiowanie informacji które aktualnie znajduj¹ siê z buforze, celem ich dalszego u¿ycia.

Argumenty to:
    1) lame_internal_flags *gfc;
    2) unsigned char *mp3buffer
    3) int mp3buffer_size_remaining
    4) Sta³a liczbowa, zapewne reprezentuje jak¹œ opcjê w funkcjê, ale dok³adnie nie wiem, nie znalaz³em cia³a funkcji.
