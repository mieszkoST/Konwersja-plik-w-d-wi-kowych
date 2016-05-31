save_gain_values(lame_internal_flags * gfc)
{
    SessionConfig_t const *const cfg = &gfc->cfg;
    RpgStateVar_t const *const rsv = &gfc->sv_rpg;
    RpgResult_t *const rov = &gfc->ov_rpg;
    /* save the ReplayGain value */
    if (cfg->findReplayGain) {
        FLOAT const RadioGain = (FLOAT) GetTitleGain(rsv->rgdata);
        if (NEQ(RadioGain, GAIN_NOT_ENOUGH_SAMPLES)) {
            rov->RadioGain = (int) floor(RadioGain * 10.0 + 0.5); /* round to nearest */
        }
        else {
            rov->RadioGain = 0;
        }
    }

    /* find the gain and scale change required for no clipping */
    if (cfg->findPeakSample) {
        rov->noclipGainChange = (int) ceil(log10(rov->PeakSample / 32767.0) * 20.0 * 10.0); /* round up */

        if (rov->noclipGainChange > 0) { /* clipping occurs */
            rov->noclipScale = floor((32767.0f / rov->PeakSample) * 100.0f) / 100.0f; /* round down */
        }
        else            /* no clipping */
            rov->noclipScale = -1.0f;
    }
}
