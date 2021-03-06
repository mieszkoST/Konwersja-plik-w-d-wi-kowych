int lame_close(lame_global_flags * gfp)  
{
    lame_internal_flags *const gfc = gfp->internal_flags;
  
    if (NULL == gfc || gfc->Class_ID != LAME_ID)
    	return -3;

    gfc->Class_ID = 0;
 
    /* Ta rutynowa procedura zwalnia cala zaalokowana pamiec we wskazniku gfc: */
    freegfc(gfc);
 
    gfp->internal_flags = NULL;
    
 	/* Jezeli jest cos jeszcze zaalokowane w gfp to zostaje to zwolnione: */
    if (gfp->lame_allocated_gfp) 
    {
    	gfp->lame_allocated_gfp = 0;
        free(gfp);
    }

    return 0;
 }
