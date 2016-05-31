static int
calcNeeded(SessionConfig_t const * cfg)
{
    int     mf_needed;
    int     pcm_samples_per_frame = 576 * cfg->mode_gr;

    /* some sanity checks */
#if ENCDELAY < MDCTDELAY
# error ENCDELAY is less than MDCTDELAY, see encoder.h
#endif
#if FFTOFFSET > BLKSIZE
# error FFTOFFSET is greater than BLKSIZE, see encoder.h
#endif

    mf_needed = BLKSIZE + pcm_samples_per_frame - FFTOFFSET; /* amount needed for FFT */
    /*mf_needed = Max(mf_needed, 286 + 576 * (1 + gfc->mode_gr)); */
    mf_needed = Max(mf_needed, 512 + pcm_samples_per_frame - 32);

    assert(MFSIZE >= mf_needed);
    
    return mf_needed;
}
