lame_init(void)
{
    lame_global_flags *gfp;
    int     ret;

    //Wywołanie jakiejś funkcji, musimy sprawdzić co jest jej zadaniem.  init_log_table();
    init_log_table();
    
    gfp = calloc(1, sizeof(lame_global_flags));
    if (gfp == NULL)
        return NULL;

    //Kolejna funkcja do przeanalizowania. lame_init_old(gfp);
    ret = lame_init_old(gfp);
    if (ret != 0) {
        free(gfp);
        return NULL;
    }

    gfp->lame_allocated_gfp = 1;
    return gfp;
}
