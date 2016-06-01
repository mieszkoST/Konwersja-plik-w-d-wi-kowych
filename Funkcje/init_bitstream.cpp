/* called by lame_init_params.  You can also call this after flush_nogap
   if you want to write new id3v2 and Xing VBR tags into the bitstream */
int
lame_init_bitstream(lame_global_flags * gfp)
{
    lame_internal_flags *const gfc = gfp->internal_flags;
    gfp->frameNum = 0;
    if (gfp->write_id3tag_automatic) {
        (void) id3tag_write_v2(gfp);
    }
    /* initialize histogram data optionally used by frontend */
    memset(gfc->bitrate_stereoMode_Hist, 0, sizeof(gfc->bitrate_stereoMode_Hist));
    memset(gfc->bitrate_blockType_Hist, 0, sizeof(gfc->bitrate_blockType_Hist));
    gfc->PeakSample = 0.0;
    /* Write initial VBR Header to bitstream and init VBR data */
    if (gfp->bWriteVbrTag)
        (void) InitVbrTag(gfp);
    return 0;
}
