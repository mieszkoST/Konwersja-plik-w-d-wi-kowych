int
lame_encode_buffer(lame_global_flags * gfp,
                   const short int buffer_l[],
                   const short int buffer_r[],
                   const int nsamples, unsigned char *mp3buf, const int mp3buf_size)
{
    lame_internal_flags *const gfc = gfp->internal_flags;
    int     i;
    sample_t *in_buffer[2];
    if (gfc->Class_ID != LAME_ID)
        return -3;
    if (nsamples == 0)
        return 0;
    if (update_inbuffer_size(gfc, nsamples) != 0) {
        return -2;
    }
    in_buffer[0] = gfc->in_buffer_0;
    in_buffer[1] = gfc->in_buffer_1;
    /* make a copy of input buffer, changing type to sample_t */
    for (i = 0; i < nsamples; i++) {
        in_buffer[0][i] = buffer_l[i];
        if (gfc->channels_in > 1)
            in_buffer[1][i] = buffer_r[i];
    }
    return lame_encode_buffer_sample_t(gfp, in_buffer[0], in_buffer[1],
                                       nsamples, mp3buf, mp3buf_size);
}
int
lame_encode_buffer_float(lame_global_flags * gfp,
                         const float buffer_l[],
                         const float buffer_r[],
                         const int nsamples, unsigned char *mp3buf, const int mp3buf_size)
{
    lame_internal_flags *const gfc = gfp->internal_flags;
    int     i;
    sample_t *in_buffer[2];
    if (gfc->Class_ID != LAME_ID)
        return -3;
    if (nsamples == 0)
        return 0;
    if (update_inbuffer_size(gfc, nsamples) != 0) {
        return -2;
    }
    in_buffer[0] = gfc->in_buffer_0;
    in_buffer[1] = gfc->in_buffer_1;
    /* make a copy of input buffer, changing type to sample_t */
    for (i = 0; i < nsamples; i++) {
        in_buffer[0][i] = buffer_l[i];
        if (gfc->channels_in > 1)
            in_buffer[1][i] = buffer_r[i];
    }
    return lame_encode_buffer_sample_t(gfp, in_buffer[0], in_buffer[1],
                                       nsamples, mp3buf, mp3buf_size);
}
