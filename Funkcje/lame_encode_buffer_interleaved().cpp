int
lame_encode_buffer_interleaved(lame_global_flags * gfp,
                               short int pcm[], int nsamples,
                               unsigned char *mp3buf, int mp3buf_size)
{
    /* input is assumed to be normalized to +/- MAX_SHORT for full scale */
    return lame_encode_buffer_template(gfp, pcm, pcm+1, nsamples, mp3buf, mp3buf_size, pcm_short_type, 2, 1.0);
}


static int
lame_encode_buffer_template(lame_global_flags * gfp,
                            void const* buffer_l, void const* buffer_r, const int nsamples,
                            unsigned char *mp3buf, const int mp3buf_size, enum PCMSampleType pcm_type, int aa, FLOAT norm)
{
    if (is_lame_global_flags_valid(gfp)) {
        lame_internal_flags *const gfc = gfp->internal_flags;
        if (is_lame_internal_flags_valid(gfc)) {
            SessionConfig_t const *const cfg = &gfc->cfg;

            if (nsamples == 0)
                return 0;

            if (update_inbuffer_size(gfc, nsamples) != 0) {
                return -2;
            }
            /* make a copy of input buffer, changing type to sample_t */
            if (cfg->channels_in > 1) {
                if (buffer_l == 0 || buffer_r == 0) {
                    return 0;
                }
                lame_copy_inbuffer(gfc, buffer_l, buffer_r, nsamples, pcm_type, aa, norm);
            }
            else {
                if (buffer_l == 0) {
                    return 0;
                }
                lame_copy_inbuffer(gfc, buffer_l, buffer_l, nsamples, pcm_type, aa, norm);
            }

            return lame_encode_buffer_sample_t(gfc, nsamples, mp3buf, mp3buf_size);
        }
    }
    return -3;
}

