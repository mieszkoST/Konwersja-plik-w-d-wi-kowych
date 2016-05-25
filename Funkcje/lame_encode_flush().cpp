int
lame_encode_flush(lame_global_flags * gfp, unsigned char *mp3buffer, int mp3buffer_size)
{
    lame_internal_flags *gfc;
    SessionConfig_t const *cfg;
    EncStateVar_t *esv;
    short int buffer[2][1152];
    int     imp3 = 0, mp3count, mp3buffer_size_remaining;

    /* we always add POSTDELAY=288 padding to make sure granule with real
     * data can be complety decoded (because of 50% overlap with next granule */
    int     end_padding;
    int     frames_left;
    int     samples_to_encode;
    int     pcm_samples_per_frame;
    int     mf_needed;
    int     is_resampling_necessary;
    double  resample_ratio = 1;

    //Przeanalizować: is_lame_global_flags_valid(gfp) oraz is_lame_internal_flags_valid(gfc)

    //is_lame_global_flags_valid(gfp) - Wydaje mi sie że funckja ta sprawdza poprawność argumentu gfp który wysyłamy do funkcji.
    //Jeśli jest on niepoprawny zwraca -3, wartość -3 zapewne jest po to, aby ew. kontrolować na jakim etapie mamy bład.
    if (!is_lame_global_flags_valid(gfp)) {
        return -3;
    }

    //Jeśli gpf jest poprawce, wskaźnik gfc ustawiamy na składową gfp o naziwe internal_flags.
    //Po raz kolejny mamy do czynienia z strukturą lame_global_flags. Musimy się dowiedzieć dokładnie co ona zawiera.
    gfc = gfp->internal_flags;

    //Kolejna funkcja do sprawdzania poprawności argumentu ? To mi sugeruje nazwa, oraz przerwanie pracy w przypadku błedu ( return -3 )
    if (!is_lame_internal_flags_valid(gfc)) {
        return -3;
    }

    //Odwołonie i przypisanie do nieznanych nam składowych. Kolejny szczegoł wymagający analizy.
    cfg = &gfc->cfg;
    esv = &gfc->sv_enc;

    /* Was flush already called? */

    //Jeśli liczba 'próbek' do zdekodowania jest wartością niepoprawną, zwracamy 0;
    if (esv->mf_samples_to_encode < 1) {
        return 0;
    }

    //Obliczenie liczby 'próbek' na jedną klatkę formatu pcm.
    pcm_samples_per_frame = 576 * cfg->mode_gr;

    //Przeanalizować: calcNeeded(cfg);
    //Ta funkcja wylicza potrzebną wartość czegoś ( pytanie czego? ) i zapisuje ją do mf_needed.
    //Trzeba będzie zajrzeć w ciało calcNeeded aby się przekonać co dokłądnie jest liczone.
    mf_needed = calcNeeded(cfg);

    //Wyliczenie liczby 'próbek' potrzebnych do zdekodowania.
    samples_to_encode = esv->mf_samples_to_encode - POSTDELAY;

    //Przeanalizować: memset(buffer, 0, sizeof(buffer));
    //Przeoczona funkcja typu void:  memset(buffer, 0, sizeof(buffer)); - Również do analizy.
    memset(buffer, 0, sizeof(buffer));
    mp3count = 0;

    //Przeanalizować: isResamplingNecessary(cfg);
    //Resampling – transformacja bitmapy polegająca na zmianie liczby jej pikseli.
    //Efektem resamplingu jest rzeczywiste powiększenie, pomniejszenie lub zmiana proporcji, ale także i obrót obrazu bitmapowego.
    //Tutaj sprawdzamy że resampling jest w naszym przypadku potrzebny, jeśli tak, to jest on wykonywany.
    //Tutaj polega on na wyliczeniu stosunku częstotliwości próbkowania na wejściu, do częstotliwości na wyjściu.
    //Oraz odpowiednim zmodyfikowaniu liczby 'próbek' koniecznych do zdekodowania.
    is_resampling_necessary = isResamplingNecessary(cfg);
    if (is_resampling_necessary) {
        resample_ratio = (double)cfg->samplerate_in / (double)cfg->samplerate_out;
        /* delay due to resampling; needs to be fixed, if resampling code gets changed */
        samples_to_encode += 16. / resample_ratio;
    }

    //Czym jest end_padding ? Wg co znaczy słowo 'padding' - przyznam szczerze, że nie mam pojęcia ;-)
    //Aby dalej kontunuować tą analizę, muszę to wiedzieć, gdyż widze że to dość znaczący parametr.

    end_padding = pcm_samples_per_frame - (samples_to_encode % pcm_samples_per_frame);
    if (end_padding < 576)
        end_padding += pcm_samples_per_frame;
    gfc->ov_enc.encoder_padding = end_padding;

    frames_left = (samples_to_encode + end_padding) / pcm_samples_per_frame;
    while (frames_left > 0 && imp3 >= 0) {
        int const frame_num = gfc->ov_enc.frame_number;
        int     bunch = mf_needed - esv->mf_size;

        bunch *= resample_ratio;
        if (bunch > 1152) bunch = 1152;
        if (bunch < 1) bunch = 1;

        mp3buffer_size_remaining = mp3buffer_size - mp3count;

        /* if user specifed buffer size = 0, dont check size */
        if (mp3buffer_size == 0)
            mp3buffer_size_remaining = 0;

        /* send in a frame of 0 padding until all internal sample buffers
         * are flushed
         */

        //Przeanalizować: lame_encode_buffer()
        imp3 = lame_encode_buffer(gfp, buffer[0], buffer[1], bunch,
                                  mp3buffer, mp3buffer_size_remaining);

        mp3buffer += imp3;
        mp3count += imp3;
        frames_left -= ((frame_num != gfc->ov_enc.frame_number) ? 1 : 0);
    }
    /* Set esv->mf_samples_to_encode to 0, so we may detect
     * and break loops calling it more than once in a row.
     */
    esv->mf_samples_to_encode = 0;

    if (imp3 < 0) {
        /* some type of fatal error */
        return imp3;
    }

    mp3buffer_size_remaining = mp3buffer_size - mp3count;
    /* if user specifed buffer size = 0, dont check size */
    if (mp3buffer_size == 0)
        mp3buffer_size_remaining = 0;

    /* mp3 related stuff.  bit buffer might still contain some mp3 data */

    //Przeanalizować: flush_bitstream(gfc);
    //Przeanalizować: copy_buffer() oraz save_gain_values(gfc);

    flush_bitstream(gfc);
    imp3 = copy_buffer(gfc, mp3buffer, mp3buffer_size_remaining, 1);
    save_gain_values(gfc);
    if (imp3 < 0) {
        /* some type of fatal error */
        return imp3;
    }
    mp3buffer += imp3;
    mp3count += imp3;
    mp3buffer_size_remaining = mp3buffer_size - mp3count;
    /* if user specifed buffer size = 0, dont check size */
    if (mp3buffer_size == 0)
        mp3buffer_size_remaining = 0;

    if (gfp->write_id3tag_automatic) {
        /* write a id3 tag to the bitstream */
        (void) id3tag_write_v1(gfp);

        imp3 = copy_buffer(gfc, mp3buffer, mp3buffer_size_remaining, 0);

        if (imp3 < 0) {
            return imp3;
        }
        mp3count += imp3;
    }
#if 0
    {
        int const ed = gfc->ov_enc.encoder_delay;
        int const ep = gfc->ov_enc.encoder_padding;
        int const ns = (gfc->ov_enc.frame_number * pcm_samples_per_frame) - (ed + ep);
        double  duration = ns;
        duration /= cfg->samplerate_out;
        MSGF(gfc, "frames=%d\n", gfc->ov_enc.frame_number);
        MSGF(gfc, "pcm_samples_per_frame=%d\n", pcm_samples_per_frame);
        MSGF(gfc, "encoder delay=%d\n", ed);
        MSGF(gfc, "encoder padding=%d\n", ep);
        MSGF(gfc, "sample count=%d (%g)\n", ns, cfg->samplerate_in * duration);
        MSGF(gfc, "duration=%g sec\n", duration);
    }
#endif
    return mp3count;
}
