lame_init(void)
{
    //Typ zwracany to 'lame_global_flags *' - jest to pewna struktura ( wywnioskowne poniżej )
    //Również musimy sprawdzić co ona zawiera, i jaki ma to wpływ na program.
    //Nazwa funkcji sugeruje, że tutaj zaczyna się działanie funkcji z bilbioteki lame.
    //Coś jest inicjowane, nadawane są pewne wartości początkowe i przydzielana jest pamięc.
    //Są tutaj kolejne funkcje, które mają z pewnością jakieś ważne zadanie.

    lame_global_flags *gfp;
    int     ret;

    //Wywołanie jakiejś funkcji, musimy sprawdzić co jest jej zadaniem.  init_log_table();
    init_log_table();

    //Alokacja pamięci dla obiektu, na który wskazuje gfp i sprawdzenie poprawoności alokacji.
    gfp = calloc(1, sizeof(lame_global_flags));
    if (gfp == NULL)
        return NULL;

    //Kolejna funkcja do przeanalizowania. lame_init_old(gfp);
    //Wartośc jaką zwraca to int, ponieważ to co zwraca jest przypisywane do zmiennej ret.
    //Zmienna ret służy tu jak widać tylko do sprawdzenia czy funkcja lame_init_old() wykonała swoje zadanie bezproblemowo.
    //Jeśli tak się nie dzieje, zwalniana jest pamięc i zwracany jest NULL.
    ret = lame_init_old(gfp);
    if (ret != 0) {
        free(gfp);
        return NULL;
    }

    //gfp musi być strukturą (wątpię że unią), sugeruje to specyficzne odwołanie do składowej tego obiektu. ( -> )
    //Jeśli działanie funkcji lame_init przebiegło pomyślnie, składowa lame_allocated_gfp ustawiana jest na 1.
    //Następnie zwracany jest wskaźnik gfp.
    gfp->lame_allocated_gfp = 1;
    return gfp;
}
