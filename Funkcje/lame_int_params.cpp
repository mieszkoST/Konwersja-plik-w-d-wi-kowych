// BARDZO WAZNE
// JEST TO FUNKCJA, KTORA JAK NARAZIE ZNALAZLEM I W MIARE
// ODPOWIADA TEMU CO SZUKAMY. W PRZYPADKU GDY ZNAJDZIECIE
// INNA FUNKCJE, KTORA JEST LEPSZA, EDYTUJCIE

int
lame_init_params(lame_global_flags * gfp)
{

    int     i;
    int     j;
    lame_internal_flags *const gfc = gfp->internal_flags;
    SessionConfig_t *const cfg = &gfc->cfg;

    gfc->class_id = 0;

    cfg->enforce_min_bitrate = gfp->VBR_hard_min;
    cfg->analysis = gfp->analysis;
    if (cfg->analysis)
        gfp->write_lame_tag = 0;

    /* some file options not allowed if output is: not specified or stdout */
    if (gfc->pinfo != NULL)
        gfp->write_lame_tag = 0; /* disable Xing VBR tag */

    /* report functions */
    gfc->report_msg = gfp->report.msgf;
    gfc->report_dbg = gfp->report.debugf;
    gfc->report_err = gfp->report.errorf;

    if (gfp->asm_optimizations.amd3dnow)
        gfc->CPU_features.AMD_3DNow = has_3DNow();
    else
        gfc->CPU_features.AMD_3DNow = 0;

    if (gfp->asm_optimizations.mmx)
        gfc->CPU_features.MMX = has_MMX();
    else
        gfc->CPU_features.MMX = 0;

    if (gfp->asm_optimizations.sse) {
        gfc->CPU_features.SSE = has_SSE();
        gfc->CPU_features.SSE2 = has_SSE2();
    }
    else {
        gfc->CPU_features.SSE = 0;
        gfc->CPU_features.SSE2 = 0;
    }


    if (NULL == gfc->ATH)
        gfc->ATH = calloc(1, sizeof(ATH_t));

    if (NULL == gfc->ATH)
        return -2;      /* maybe error codes should be enumerated in lame.h ?? */

    if (NULL == gfc->sv_rpg.rgdata)
        gfc->sv_rpg.rgdata = calloc(1, sizeof(replaygain_t));
    if (NULL == gfc->sv_rpg.rgdata) {
        freegfc(gfc);
        gfp->internal_flags = NULL;
        return -2;
    }

    cfg->error_protection = gfp->error_protection;
    cfg->copyright = gfp->copyright;
    cfg->original = gfp->original;
    cfg->extension = gfp->extension;
    cfg->emphasis = gfp->emphasis;

    cfg->channels_in = gfp->num_channels;
    if (cfg->channels_in == 1)
        gfp->mode = MONO;
    cfg->channels_out = (gfp->mode == MONO) ? 1 : 2;
    if (gfp->mode == MONO)
        gfp->force_ms = 0; /* don't allow forced mid/side stereo for mono output */
    cfg->force_ms = gfp->force_ms;

    if (gfp->VBR == vbr_off && gfp->VBR_mean_bitrate_kbps != 128 && gfp->brate == 0)
        gfp->brate = gfp->VBR_mean_bitrate_kbps;

    switch (gfp->VBR) {
    case vbr_off:
    case vbr_mtrh:
    case vbr_mt:
        /* these modes can handle free format condition */
        break;
    default:
        gfp->free_format = 0; /* mode can't be mixed with free format */
        break;
    }

    cfg->free_format = gfp->free_format;

    if (gfp->VBR == vbr_off && gfp->brate == 0) {
        /* no bitrate or compression ratio specified, use 11.025 */
        if (EQ(gfp->compression_ratio, 0))
            gfp->compression_ratio = 11.025; /* rate to compress a CD down to exactly 128000 bps */
    }

    /* find bitrate if user specify a compression ratio */
    if (gfp->VBR == vbr_off && gfp->compression_ratio > 0) {

        if (gfp->samplerate_out == 0)
            gfp->samplerate_out = map2MP3Frequency((int) (0.97 * gfp->samplerate_in)); /* round up with a margin of 3% */

        /* choose a bitrate for the output samplerate which achieves
         * specified compression ratio
         */
        gfp->brate = gfp->samplerate_out * 16 * cfg->channels_out / (1.e3 * gfp->compression_ratio);

        /* we need the version for the bitrate table look up */
        cfg->samplerate_index = SmpFrqIndex(gfp->samplerate_out, &cfg->version);

        if (!cfg->free_format) /* for non Free Format find the nearest allowed bitrate */
            gfp->brate = FindNearestBitrate(gfp->brate, cfg->version, gfp->samplerate_out);
    }
    if (gfp->samplerate_out) {
        if (gfp->samplerate_out < 16000) {
            gfp->VBR_mean_bitrate_kbps = Max(gfp->VBR_mean_bitrate_kbps, 8);
            gfp->VBR_mean_bitrate_kbps = Min(gfp->VBR_mean_bitrate_kbps, 64);
        }
        else if (gfp->samplerate_out < 32000) {
            gfp->VBR_mean_bitrate_kbps = Max(gfp->VBR_mean_bitrate_kbps, 8);
            gfp->VBR_mean_bitrate_kbps = Min(gfp->VBR_mean_bitrate_kbps, 160);
        }
        else {
            gfp->VBR_mean_bitrate_kbps = Max(gfp->VBR_mean_bitrate_kbps, 32);
            gfp->VBR_mean_bitrate_kbps = Min(gfp->VBR_mean_bitrate_kbps, 320);
        }
    }
    /* WORK IN PROGRESS */
    /* mapping VBR scale to internal VBR quality settings */
    if (gfp->samplerate_out == 0 && (gfp->VBR == vbr_mt || gfp->VBR == vbr_mtrh)) {
        float const qval = gfp->VBR_q + gfp->VBR_q_frac;
        struct q_map { int sr_a; float qa, qb, ta, tb; int lp; };
        struct q_map const m[9]
        = { {48000, 0.0,6.5,  0.0,6.5, 23700}
          , {44100, 0.0,6.5,  0.0,6.5, 21780}
          , {32000, 6.5,8.0,  5.2,6.5, 15800}
          , {24000, 8.0,8.5,  5.2,6.0, 11850}
          , {22050, 8.5,9.01, 5.2,6.5, 10892}
          , {16000, 9.01,9.4, 4.9,6.5,  7903}
          , {12000, 9.4,9.6,  4.5,6.0,  5928}
          , {11025, 9.6,9.9,  5.1,6.5,  5446}
          , { 8000, 9.9,10.,  4.9,6.5,  3952}
        };
        for (i = 2; i < 9; ++i) {
            if (gfp->samplerate_in == m[i].sr_a) {
                if (qval < m[i].qa) {
                    double d = qval / m[i].qa;
                    d = d * m[i].ta;
                    gfp->VBR_q = (int)d;
                    gfp->VBR_q_frac = d - gfp->VBR_q;
                }
            }
            if (gfp->samplerate_in >= m[i].sr_a) {
                if (m[i].qa <= qval && qval < m[i].qb) {
                    float const q_ = m[i].qb-m[i].qa;
                    float const t_ = m[i].tb-m[i].ta;
                    double d = m[i].ta + t_ * (qval-m[i].qa) / q_;
                    gfp->VBR_q = (int)d;
                    gfp->VBR_q_frac = d - gfp->VBR_q;
                    gfp->samplerate_out = m[i].sr_a;
                    if (gfp->lowpassfreq == 0) {
                        gfp->lowpassfreq = -1;
                    }
                    break;
                }
            }
        }
    }

    /****************************************************************/
    /* if a filter has not been enabled, see if we should add one: */
    /****************************************************************/
    if (gfp->lowpassfreq == 0) {
        double  lowpass = 16000;
        double  highpass;

        switch (gfp->VBR) {
        case vbr_off:{
                optimum_bandwidth(&lowpass, &highpass, gfp->brate);
                break;
            }
        case vbr_abr:{
                optimum_bandwidth(&lowpass, &highpass, gfp->VBR_mean_bitrate_kbps);
                break;
            }
        case vbr_rh:{
                int const x[11] = {
                    19500, 19000, 18600, 18000, 17500, 16000, 15600, 14900, 12500, 10000, 3950
                };
                if (0 <= gfp->VBR_q && gfp->VBR_q <= 9) {
                    double  a = x[gfp->VBR_q], b = x[gfp->VBR_q + 1], m = gfp->VBR_q_frac;
                    lowpass = linear_int(a, b, m);
                }
                else {
                    lowpass = 19500;
                }
                break;
            }
        case vbr_mtrh:
        case vbr_mt:{
                int const x[11] = {
                    24000, 19500, 18500, 18000, 17500, 17000, 16500, 15600, 15200, 7230, 3950
                };
                if (0 <= gfp->VBR_q && gfp->VBR_q <= 9) {
                    double  a = x[gfp->VBR_q], b = x[gfp->VBR_q + 1], m = gfp->VBR_q_frac;
                    lowpass = linear_int(a, b, m);
                }
                else {
                    lowpass = 21500;
                }
                break;
            }
        default:{
                int const x[11] = {
                    19500, 19000, 18500, 18000, 17500, 16500, 15500, 14500, 12500, 9500, 3950
                };
                if (0 <= gfp->VBR_q && gfp->VBR_q <= 9) {
                    double  a = x[gfp->VBR_q], b = x[gfp->VBR_q + 1], m = gfp->VBR_q_frac;
                    lowpass = linear_int(a, b, m);
                }
                else {
                    lowpass = 19500;
                }
            }
        }

        if (gfp->mode == MONO && (gfp->VBR == vbr_off || gfp->VBR == vbr_abr))
            lowpass *= 1.5;

        gfp->lowpassfreq = lowpass;
    }

    if (gfp->samplerate_out == 0) {
        if (2 * gfp->lowpassfreq > gfp->samplerate_in) {
            gfp->lowpassfreq = gfp->samplerate_in / 2;
        }
        gfp->samplerate_out = optimum_samplefreq((int) gfp->lowpassfreq, gfp->samplerate_in);
    }
    if (gfp->VBR == vbr_mt || gfp->VBR == vbr_mtrh) {
        gfp->lowpassfreq = Min(24000, gfp->lowpassfreq);
    }
    else {
        gfp->lowpassfreq = Min(20500, gfp->lowpassfreq);
    }
    gfp->lowpassfreq = Min(gfp->samplerate_out / 2, gfp->lowpassfreq);

    if (gfp->VBR == vbr_off) {
        gfp->compression_ratio = gfp->samplerate_out * 16 * cfg->channels_out / (1.e3 * gfp->brate);
    }
    if (gfp->VBR == vbr_abr) {
        gfp->compression_ratio =
            gfp->samplerate_out * 16 * cfg->channels_out / (1.e3 * gfp->VBR_mean_bitrate_kbps);
    }

    /* do not compute ReplayGain values and do not find the peak sample
       if we can't store them */
    if (!gfp->write_lame_tag) {
        gfp->findReplayGain = 0;
        gfp->decode_on_the_fly = 0;
        cfg->findPeakSample = 0;
    }

    cfg->findReplayGain = gfp->findReplayGain;
    cfg->decode_on_the_fly = gfp->decode_on_the_fly;

    if (cfg->decode_on_the_fly)
        cfg->findPeakSample = 1;

    if (cfg->findReplayGain) {
        if (InitGainAnalysis(gfc->sv_rpg.rgdata, gfp->samplerate_out) == INIT_GAIN_ANALYSIS_ERROR) {
            freegfc(gfc);
            gfp->internal_flags = NULL;
            return -6;
        }
    }

#ifdef DECODE_ON_THE_FLY
    if (cfg->decode_on_the_fly && !gfp->decode_only) {
        if (gfc->hip) {
            hip_decode_exit(gfc->hip);
        }
        gfc->hip = hip_decode_init();
        /* report functions */
        hip_set_errorf(gfc->hip, gfp->report.errorf);
        hip_set_debugf(gfc->hip, gfp->report.debugf);
        hip_set_msgf(gfc->hip, gfp->report.msgf);
    }
#endif

    cfg->disable_reservoir = gfp->disable_reservoir;
    cfg->lowpassfreq = gfp->lowpassfreq;
    cfg->highpassfreq = gfp->highpassfreq;
    cfg->samplerate_in = gfp->samplerate_in;
    cfg->samplerate_out = gfp->samplerate_out;
    cfg->mode_gr = cfg->samplerate_out <= 24000 ? 1 : 2; /* Number of granules per frame */
    gfc->ov_enc.encoder_delay = ENCDELAY;


    /*
     *  sample freq       bitrate     compression ratio
     *     [kHz]      [kbps/channel]   for 16 bit input
     *     44.1            56               12.6
     *     44.1            64               11.025
     *     44.1            80                8.82
     *     22.05           24               14.7
     *     22.05           32               11.025
     *     22.05           40                8.82
     *     16              16               16.0
     *     16              24               10.667
     *
     */
    /*
     *  For VBR, take a guess at the compression_ratio.
     *  For example:
     *
     *    VBR_q    compression     like
     *     -        4.4         320 kbps/44 kHz
     *   0...1      5.5         256 kbps/44 kHz
     *     2        7.3         192 kbps/44 kHz
     *     4        8.8         160 kbps/44 kHz
     *     6       11           128 kbps/44 kHz
     *     9       14.7          96 kbps
     *
     *  for lower bitrates, downsample with --resample
     */

    switch (gfp->VBR) {
    case vbr_mt:
    case vbr_rh:
    case vbr_mtrh:
        {
            /*numbers are a bit strange, but they determine the lowpass value */
            FLOAT const cmp[] = { 5.7, 6.5, 7.3, 8.2, 10, 11.9, 13, 14, 15, 16.5 };
            gfp->compression_ratio = cmp[gfp->VBR_q];
        }
        break;
    case vbr_abr:
        gfp->compression_ratio =
            cfg->samplerate_out * 16 * cfg->channels_out / (1.e3 * gfp->VBR_mean_bitrate_kbps);
        break;
    default:
        gfp->compression_ratio = cfg->samplerate_out * 16 * cfg->channels_out / (1.e3 * gfp->brate);
        break;
    }


    /* mode = -1 (not set by user) or
     * mode = MONO (because of only 1 input channel).
     * If mode has not been set, then select J-STEREO
     */
    if (gfp->mode == NOT_SET) {
        gfp->mode = JOINT_STEREO;
    }

    cfg->mode = gfp->mode;


    /* apply user driven high pass filter */
    if (cfg->highpassfreq > 0) {
        cfg->highpass1 = 2. * cfg->highpassfreq;

        if (gfp->highpasswidth >= 0)
            cfg->highpass2 = 2. * (cfg->highpassfreq + gfp->highpasswidth);
        else            /* 0% above on default */
            cfg->highpass2 = (1 + 0.00) * 2. * cfg->highpassfreq;

        cfg->highpass1 /= cfg->samplerate_out;
        cfg->highpass2 /= cfg->samplerate_out;
    }
    else {
        cfg->highpass1 = 0;
        cfg->highpass2 = 0;
    }
    /* apply user driven low pass filter */
    cfg->lowpass1 = 0;
    cfg->lowpass2 = 0;
    if (cfg->lowpassfreq > 0 && cfg->lowpassfreq < (cfg->samplerate_out / 2) ) {
        cfg->lowpass2 = 2. * cfg->lowpassfreq;
        if (gfp->lowpasswidth >= 0) {
            cfg->lowpass1 = 2. * (cfg->lowpassfreq - gfp->lowpasswidth);
            if (cfg->lowpass1 < 0) /* has to be >= 0 */
                cfg->lowpass1 = 0;
        }
        else {          /* 0% below on default */
            cfg->lowpass1 = (1 - 0.00) * 2. * cfg->lowpassfreq;
        }
        cfg->lowpass1 /= cfg->samplerate_out;
        cfg->lowpass2 /= cfg->samplerate_out;
    }




  /**********************************************************************/
    /* compute info needed for polyphase filter (filter type==0, default) */
  /**********************************************************************/
    lame_init_params_ppflt(gfc);


  /*******************************************************
   * samplerate and bitrate index
   *******************************************************/
    cfg->samplerate_index = SmpFrqIndex(cfg->samplerate_out, &cfg->version);
    if (cfg->samplerate_index < 0) {
        freegfc(gfc);
        gfp->internal_flags = NULL;
        return -1;
    }

    if (gfp->VBR == vbr_off) {
        if (cfg->free_format) {
            gfc->ov_enc.bitrate_index = 0;
        }
        else {
            gfp->brate = FindNearestBitrate(gfp->brate, cfg->version, cfg->samplerate_out);
            gfc->ov_enc.bitrate_index = BitrateIndex(gfp->brate, cfg->version, cfg->samplerate_out);
            if (gfc->ov_enc.bitrate_index <= 0) {
                freegfc(gfc);
                gfp->internal_flags = NULL;
                return -1;
            }
        }
    }
    else {
        gfc->ov_enc.bitrate_index = 1;
    }

    init_bit_stream_w(gfc);

    j = cfg->samplerate_index + (3 * cfg->version) + 6 * (cfg->samplerate_out < 16000);
    for (i = 0; i < SBMAX_l + 1; i++)
        gfc->scalefac_band.l[i] = sfBandIndex[j].l[i];

    for (i = 0; i < PSFB21 + 1; i++) {
        int const size = (gfc->scalefac_band.l[22] - gfc->scalefac_band.l[21]) / PSFB21;
        int const start = gfc->scalefac_band.l[21] + i * size;
        gfc->scalefac_band.psfb21[i] = start;
    }
    gfc->scalefac_band.psfb21[PSFB21] = 576;

    for (i = 0; i < SBMAX_s + 1; i++)
        gfc->scalefac_band.s[i] = sfBandIndex[j].s[i];

    for (i = 0; i < PSFB12 + 1; i++) {
        int const size = (gfc->scalefac_band.s[13] - gfc->scalefac_band.s[12]) / PSFB12;
        int const start = gfc->scalefac_band.s[12] + i * size;
        gfc->scalefac_band.psfb12[i] = start;
    }
    gfc->scalefac_band.psfb12[PSFB12] = 192;

    /* determine the mean bitrate for main data */
    if (cfg->mode_gr == 2) /* MPEG 1 */
        cfg->sideinfo_len = (cfg->channels_out == 1) ? 4 + 17 : 4 + 32;
    else                /* MPEG 2 */
        cfg->sideinfo_len = (cfg->channels_out == 1) ? 4 + 9 : 4 + 17;

    if (cfg->error_protection)
        cfg->sideinfo_len += 2;

    gfc->class_id = LAME_ID;

    {
        int     k;

        for (k = 0; k < 19; k++)
            gfc->sv_enc.pefirbuf[k] = 700 * cfg->mode_gr * cfg->channels_out;

        if (gfp->ATHtype == -1)
            gfp->ATHtype = 4;
    }

    assert(gfp->VBR_q <= 9);
    assert(gfp->VBR_q >= 0);

    switch (gfp->VBR) {

    case vbr_mt:
    case vbr_mtrh:{
            if (gfp->strict_ISO < 0) {
                gfp->strict_ISO = MDB_MAXIMUM;
            }
            if (gfp->useTemporal < 0) {
                gfp->useTemporal = 0; /* off by default for this VBR mode */
            }

            (void) apply_preset(gfp, 500 - (gfp->VBR_q * 10), 0);
            /*  The newer VBR code supports only a limited
               subset of quality levels:
               9-5=5 are the same, uses x^3/4 quantization
               4-0=0 are the same  5 plus best huffman divide code
             */
            if (gfp->quality < 0)
                gfp->quality = LAME_DEFAULT_QUALITY;
            if (gfp->quality < 5)
                gfp->quality = 0;
            if (gfp->quality > 7)
                gfp->quality = 7;

            /*  sfb21 extra only with MPEG-1 at higher sampling rates
             */
            if (gfp->experimentalY)
                gfc->sv_qnt.sfb21_extra = 0;
            else
                gfc->sv_qnt.sfb21_extra = (cfg->samplerate_out > 44000);

            gfc->iteration_loop = VBR_new_iteration_loop;
            break;

        }
    case vbr_rh:{

            (void) apply_preset(gfp, 500 - (gfp->VBR_q * 10), 0);

            /*  sfb21 extra only with MPEG-1 at higher sampling rates
             */
            if (gfp->experimentalY)
                gfc->sv_qnt.sfb21_extra = 0;
            else
                gfc->sv_qnt.sfb21_extra = (cfg->samplerate_out > 44000);

            /*  VBR needs at least the output of GPSYCHO,
             *  so we have to garantee that by setting a minimum
             *  quality level, actually level 6 does it.
             *  down to level 6
             */
            if (gfp->quality > 6)
                gfp->quality = 6;


            if (gfp->quality < 0)
                gfp->quality = LAME_DEFAULT_QUALITY;

            gfc->iteration_loop = VBR_old_iteration_loop;
            break;
        }

    default:           /* cbr/abr */  {
            vbr_mode vbrmode;

            /*  no sfb21 extra with CBR code
             */
            gfc->sv_qnt.sfb21_extra = 0;

            if (gfp->quality < 0)
                gfp->quality = LAME_DEFAULT_QUALITY;


            vbrmode = gfp->VBR;
            if (vbrmode == vbr_off)
                (void) lame_set_VBR_mean_bitrate_kbps(gfp, gfp->brate);
            /* second, set parameters depending on bitrate */
            (void) apply_preset(gfp, gfp->VBR_mean_bitrate_kbps, 0);
            gfp->VBR = vbrmode;

            if (vbrmode == vbr_off) {
                gfc->iteration_loop = CBR_iteration_loop;
            }
            else {
                gfc->iteration_loop = ABR_iteration_loop;
            }
            break;
        }
    }

    /*initialize default values common for all modes */

    gfc->sv_qnt.mask_adjust = gfp->maskingadjust;
    gfc->sv_qnt.mask_adjust_short = gfp->maskingadjust_short;

    /*  just another daily changing developer switch  */
    if (gfp->tune) {
        gfc->sv_qnt.mask_adjust += gfp->tune_value_a;
        gfc->sv_qnt.mask_adjust_short += gfp->tune_value_a;
    }


    if (gfp->VBR != vbr_off) { /* choose a min/max bitrate for VBR */
        /* if the user didn't specify VBR_max_bitrate: */
        cfg->vbr_min_bitrate_index = 1; /* default: allow   8 kbps (MPEG-2) or  32 kbps (MPEG-1) */
        cfg->vbr_max_bitrate_index = 14; /* default: allow 160 kbps (MPEG-2) or 320 kbps (MPEG-1) */
        if (cfg->samplerate_out < 16000)
            cfg->vbr_max_bitrate_index = 8; /* default: allow 64 kbps (MPEG-2.5) */
        if (gfp->VBR_min_bitrate_kbps) {
            gfp->VBR_min_bitrate_kbps =
                FindNearestBitrate(gfp->VBR_min_bitrate_kbps, cfg->version, cfg->samplerate_out);
            cfg->vbr_min_bitrate_index =
                BitrateIndex(gfp->VBR_min_bitrate_kbps, cfg->version, cfg->samplerate_out);
            if (cfg->vbr_min_bitrate_index < 0)
                return -1;
        }
        if (gfp->VBR_max_bitrate_kbps) {
            gfp->VBR_max_bitrate_kbps =
                FindNearestBitrate(gfp->VBR_max_bitrate_kbps, cfg->version, cfg->samplerate_out);
            cfg->vbr_max_bitrate_index =
                BitrateIndex(gfp->VBR_max_bitrate_kbps, cfg->version, cfg->samplerate_out);
            if (cfg->vbr_max_bitrate_index < 0)
                return -1;
        }
        gfp->VBR_min_bitrate_kbps = bitrate_table[cfg->version][cfg->vbr_min_bitrate_index];
        gfp->VBR_max_bitrate_kbps = bitrate_table[cfg->version][cfg->vbr_max_bitrate_index];
        gfp->VBR_mean_bitrate_kbps =
            Min(bitrate_table[cfg->version][cfg->vbr_max_bitrate_index],
                gfp->VBR_mean_bitrate_kbps);
        gfp->VBR_mean_bitrate_kbps =
            Max(bitrate_table[cfg->version][cfg->vbr_min_bitrate_index],
                gfp->VBR_mean_bitrate_kbps);
    }

    cfg->preset = gfp->preset;
    cfg->write_lame_tag = gfp->write_lame_tag;
    cfg->vbr = gfp->VBR;
    gfc->sv_qnt.substep_shaping = gfp->substep_shaping;
    cfg->noise_shaping = gfp->noise_shaping;
    cfg->subblock_gain = gfp->subblock_gain;
    cfg->use_best_huffman = gfp->use_best_huffman;
    cfg->avg_bitrate = gfp->brate;
    cfg->vbr_avg_bitrate_kbps = gfp->VBR_mean_bitrate_kbps;
    cfg->compression_ratio = gfp->compression_ratio;

    /* initialize internal qval settings */
    lame_init_qval(gfp);


    /*  automatic ATH adjustment on
     */
    if (gfp->athaa_type < 0)
        gfc->ATH->use_adjust = 3;
    else
        gfc->ATH->use_adjust = gfp->athaa_type;


    /* initialize internal adaptive ATH settings  -jd */
    gfc->ATH->aa_sensitivity_p = pow(10.0, gfp->athaa_sensitivity / -10.0);


    if (gfp->short_blocks == short_block_not_set) {
        gfp->short_blocks = short_block_allowed;
    }

    /*Note Jan/2003: Many hardware decoders cannot handle short blocks in regular
       stereo mode unless they are coupled (same type in both channels)
       it is a rare event (1 frame per min. or so) that LAME would use
       uncoupled short blocks, so lets turn them off until we decide
       how to handle this.  No other encoders allow uncoupled short blocks,
       even though it is in the standard.  */
    /* rh 20040217: coupling makes no sense for mono and dual-mono streams
     */
    if (gfp->short_blocks == short_block_allowed
        && (cfg->mode == JOINT_STEREO || cfg->mode == STEREO)) {
        gfp->short_blocks = short_block_coupled;
    }

    cfg->short_blocks = gfp->short_blocks;


    if (lame_get_quant_comp(gfp) < 0)
        (void) lame_set_quant_comp(gfp, 1);
    if (lame_get_quant_comp_short(gfp) < 0)
        (void) lame_set_quant_comp_short(gfp, 0);

    if (lame_get_msfix(gfp) < 0)
        lame_set_msfix(gfp, 0);

    /* select psychoacoustic model */
    (void) lame_set_exp_nspsytune(gfp, lame_get_exp_nspsytune(gfp) | 1);

    if (gfp->ATHtype < 0)
        gfp->ATHtype = 4;

    if (gfp->ATHcurve < 0)
        gfp->ATHcurve = 4;

    if (gfp->interChRatio < 0)
        gfp->interChRatio = 0;

    if (gfp->useTemporal < 0)
        gfp->useTemporal = 1; /* on by default */


    cfg->interChRatio = gfp->interChRatio;
    cfg->msfix = gfp->msfix;
    cfg->ATH_offset_db = 0-gfp->ATH_lower_db;
    cfg->ATH_offset_factor = powf(10.f, cfg->ATH_offset_db * 0.1f);
    cfg->ATHcurve = gfp->ATHcurve;
    cfg->ATHtype = gfp->ATHtype;
    cfg->ATHonly = gfp->ATHonly;
    cfg->ATHshort = gfp->ATHshort;
    cfg->noATH = gfp->noATH;

    cfg->quant_comp = gfp->quant_comp;
    cfg->quant_comp_short = gfp->quant_comp_short;

    cfg->use_temporal_masking_effect = gfp->useTemporal;
    cfg->use_safe_joint_stereo = gfp->exp_nspsytune & 2;
    {
        cfg->adjust_bass_db = (gfp->exp_nspsytune >> 2) & 63;
        if (cfg->adjust_bass_db >= 32.f)
            cfg->adjust_bass_db -= 64.f;
        cfg->adjust_bass_db *= 0.25f;

        cfg->adjust_alto_db = (gfp->exp_nspsytune >> 8) & 63;
        if (cfg->adjust_alto_db >= 32.f)
            cfg->adjust_alto_db -= 64.f;
        cfg->adjust_alto_db *= 0.25f;

        cfg->adjust_treble_db = (gfp->exp_nspsytune >> 14) & 63;
        if (cfg->adjust_treble_db >= 32.f)
            cfg->adjust_treble_db -= 64.f;
        cfg->adjust_treble_db *= 0.25f;

        /*  to be compatible with Naoki's original code, the next 6 bits
         *  define only the amount of changing treble for sfb21 */
        cfg->adjust_sfb21_db = (gfp->exp_nspsytune >> 20) & 63;
        if (cfg->adjust_sfb21_db >= 32.f)
            cfg->adjust_sfb21_db -= 64.f;
        cfg->adjust_sfb21_db *= 0.25f;
        cfg->adjust_sfb21_db += cfg->adjust_treble_db;
    }

    /* Setting up the PCM input data transform matrix, to apply
     * user defined re-scaling, and or two-to-one channel downmix.
     */
    {
        FLOAT   m[2][2] = { {1.0f, 0.0f}, {0.0f, 1.0f} };

        /* user selected scaling of the samples */
        m[0][0] *= gfp->scale;
        m[0][1] *= gfp->scale;
        m[1][0] *= gfp->scale;
        m[1][1] *= gfp->scale;
        /* user selected scaling of the channel 0 (left) samples */
        m[0][0] *= gfp->scale_left;
        m[0][1] *= gfp->scale_left;
        /* user selected scaling of the channel 1 (right) samples */
        m[1][0] *= gfp->scale_right;
        m[1][1] *= gfp->scale_right;
        /* Downsample to Mono if 2 channels in and 1 channel out */
        if (cfg->channels_in == 2 && cfg->channels_out == 1) {
            m[0][0] = 0.5f * (m[0][0] + m[1][0]);
            m[0][1] = 0.5f * (m[0][1] + m[1][1]);
            m[1][0] = 0;
            m[1][1] = 0;
        }
        cfg->pcm_transform[0][0] = m[0][0];
        cfg->pcm_transform[0][1] = m[0][1];
        cfg->pcm_transform[1][0] = m[1][0];
        cfg->pcm_transform[1][1] = m[1][1];
    }

    /* padding method as described in
     * "MPEG-Layer3 / Bitstream Syntax and Decoding"
     * by Martin Sieler, Ralph Sperschneider
     *
     * note: there is no padding for the very first frame
     *
     * Robert Hegemann 2000-06-22
     */
    gfc->sv_enc.slot_lag = gfc->sv_enc.frac_SpF = 0;
    if (cfg->vbr == vbr_off)
        gfc->sv_enc.slot_lag = gfc->sv_enc.frac_SpF
            = ((cfg->version + 1) * 72000L * cfg->avg_bitrate) % cfg->samplerate_out;

    (void) lame_init_bitstream(gfp);

    iteration_init(gfc);
    (void) psymodel_init(gfp);

    cfg->buffer_constraint = get_max_frame_buffer_size_by_constraint(cfg, gfp->strict_ISO);
    return 0;
}
