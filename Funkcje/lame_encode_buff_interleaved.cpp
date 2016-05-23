int lame_encode_buffer_interleaved(lame_global_flags * gfp, short int buffer[], int nsamples, unsigned char *mp3buf, int mp3buf_size)
{
    lame_internal_flags *const gfc = gfp->internal_flags;
    int i;
    sample_t *in_buffer[2];
 
    if (update_inbuffer_size(gfc, nsamples) != 0) 
		return -2;
    
 
    in_buffer[0] = gfc->in_buffer_0;
    in_buffer[1] = gfc->in_buffer_1;
 
    for (i = 0; i < nsamples; i++) 
	{
        in_buffer[0][i] = buffer[2 * i];
        in_buffer[1][i] = buffer[2 * i + 1];
    }
    return lame_encode_buffer_sample_t(gfp, in_buffer[0], in_buffer[1], nsamples, mp3buf, mp3buf_size);
}
