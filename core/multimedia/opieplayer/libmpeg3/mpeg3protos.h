/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#ifndef MPEG3PROTOS_H
#define MPEG3PROTOS_H

#if defined(__cplusplus)
extern "C" {
#endif

/* CSS */

mpeg3_css_t* mpeg3_new_css();

/* DEMUX */

mpeg3_demuxer_t* mpeg3_new_demuxer(mpeg3_t *file, int do_audio, int do_video, int stream_id);
int mpeg3_delete_demuxer(mpeg3_demuxer_t *demuxer);
int mpeg3demux_read_data(mpeg3_demuxer_t *demuxer, unsigned char *output, long size);
unsigned int mpeg3demux_read_int32(mpeg3_demuxer_t *demuxer);
unsigned int mpeg3demux_read_int24(mpeg3_demuxer_t *demuxer);
unsigned int mpeg3demux_read_int16(mpeg3_demuxer_t *demuxer);
double mpeg3demux_length(mpeg3_demuxer_t *demuxer);
mpeg3_demuxer_t* mpeg3_get_demuxer(mpeg3_t *file);
long mpeg3demux_tell(mpeg3_demuxer_t *demuxer);
double mpeg3demux_tell_percentage(mpeg3_demuxer_t *demuxer);
double mpeg3demux_get_time(mpeg3_demuxer_t *demuxer);
int mpeg3demux_eof(mpeg3_demuxer_t *demuxer);
int mpeg3demux_bof(mpeg3_demuxer_t *demuxer);
int mpeg3demux_copy_titles(mpeg3_demuxer_t *dst, mpeg3_demuxer_t *src);
int mpeg3demux_create_title(mpeg3_demuxer_t *demuxer, int timecode_search, FILE *toc);
long mpeg3demuxer_total_bytes(mpeg3_demuxer_t *demuxer);
int mpeg3demux_seek_byte(mpeg3_demuxer_t *demuxer, long byte);
int mpeg3demux_seek_time(mpeg3_demuxer_t *demuxer, double new_time);
int mpeg3demux_seek_percentage(mpeg3_demuxer_t *demuxer, double percentage);
int mpeg3demux_print_streams(mpeg3_demuxer_t *demuxer, FILE *toc);
int mpeg3demux_print_timecodes(mpeg3_title_t *title, FILE *output);
int mpeg3demux_read_titles(mpeg3_demuxer_t *demuxer);
int mpeg3demux_open_title(mpeg3_demuxer_t *demuxer, int title_number);

/* TITLE */

mpeg3_title_t* mpeg3_new_title(mpeg3_t *file, char *path);
int mpeg3_delete_title(mpeg3_title_t *title);
int mpeg3_copy_title(mpeg3_title_t *dst, mpeg3_title_t *src);


/* ATRACK */

mpeg3_atrack_t* mpeg3_new_atrack(mpeg3_t *file, int stream_id, int is_ac3, mpeg3_demuxer_t *demuxer);
int mpeg3_delete_atrack(mpeg3_t *file, mpeg3_atrack_t *atrack);

/* VTRACK */

mpeg3_vtrack_t* mpeg3_new_vtrack(mpeg3_t *file, int stream_id, mpeg3_demuxer_t *demuxer);
int mpeg3_delete_vtrack(mpeg3_t *file, mpeg3_vtrack_t *vtrack);

/* AUDIO */
mpeg3audio_t* mpeg3audio_new(mpeg3_t *file, mpeg3_atrack_t *track, int is_ac3);
int mpeg3audio_delete(mpeg3audio_t *audio);
int mpeg3audio_seek_sample(mpeg3audio_t *audio, long sample);
int mpeg3audio_seek_percentage(mpeg3audio_t *audio, double percentage);
int mpeg3audio_decode_audio(mpeg3audio_t *audio,
                mpeg3_real_t *output_f,
                short *output_i, int sampleSpacing,
                int channel,
                long start_position,
                long len);
int mpeg3audio_read_raw(mpeg3audio_t *audio, unsigned char *output, long *size, long max_size);
int mpeg3audio_read_ac3_header(mpeg3audio_t *audio);
int mpeg3audio_read_pcm_header(mpeg3audio_t *audio);
int mpeg3audio_synth_mono(mpeg3audio_t *audio, mpeg3_real_t *bandPtr, mpeg3_real_t *samples, int *pnt);
int mpeg3audio_synth_stereo(mpeg3audio_t *audio, mpeg3_real_t *bandPtr, int channel, mpeg3_real_t *out, int *pnt);
int mpeg3audio_replace_buffer(mpeg3audio_t *audio, long new_allocation);
int mpeg3audio_ac3_exponent_unpack(mpeg3audio_t *audio,
                mpeg3_ac3bsi_t *bsi,
                mpeg3_ac3audblk_t *audblk);
int mpeg3audio_ac3_bit_allocate(mpeg3audio_t *audio,
                unsigned int fscod,
                mpeg3_ac3bsi_t *bsi,
                mpeg3_ac3audblk_t *audblk);
int mpeg3audio_ac3_coeff_unpack(mpeg3audio_t *audio,
        mpeg3_ac3bsi_t *bsi,
        mpeg3_ac3audblk_t *audblk,
        mpeg3ac3_stream_samples_t samples);
int mpeg3audio_ac3_imdct(mpeg3audio_t *audio,
                mpeg3_ac3bsi_t *bsi,
                mpeg3_ac3audblk_t *audblk,
                mpeg3ac3_stream_samples_t samples);
int mpeg3audio_replace_buffer(mpeg3audio_t *audio, long new_allocation);
int mpeg3audio_dct36(mpeg3_real_t *inbuf, mpeg3_real_t *o1, mpeg3_real_t *o2, mpeg3_real_t *wintab, mpeg3_real_t *tsbuf);
int mpeg3audio_dct12(mpeg3_real_t *in,mpeg3_real_t *rawout1,mpeg3_real_t *rawout2,register mpeg3_real_t *wi,register mpeg3_real_t *ts);
int mpeg3audio_read_header(mpeg3audio_t *audio);
int mpeg3audio_do_ac3(mpeg3audio_t *audio);
int mpeg3audio_dolayer2(mpeg3audio_t *audio);
int mpeg3audio_dolayer3(mpeg3audio_t *audio);
int mpeg3audio_do_pcm(mpeg3audio_t *audio);
int mpeg3audio_dct64(mpeg3_real_t *a, mpeg3_real_t *b, mpeg3_real_t *c);
int mpeg3audio_reset_synths(mpeg3audio_t *audio);
int mpeg3audio_prev_header(mpeg3audio_t *audio);
int mpeg3audio_read_layer3_frame(mpeg3audio_t *audio);
int mpeg3audio_new_decode_tables(mpeg3audio_t *audio);
int mpeg3audio_imdct_init(mpeg3audio_t *audio);


/* VIDEO */
mpeg3video_t* mpeg3video_new(mpeg3_t *file, mpeg3_vtrack_t *track);
int mpeg3video_delete(mpeg3video_t *video);
int mpeg3video_read_frame(mpeg3video_t *video, 
		long frame_number, 
		unsigned char **output_rows,
		int in_x, 
		int in_y, 
		int in_w, 
		int in_h, 
		int out_w, 
		int out_h, 
		int color_model);
int mpeg3video_set_cpus(mpeg3video_t *video, int cpus);
int mpeg3video_set_mmx(mpeg3video_t *video, int use_mmx);
int mpeg3video_seek(mpeg3video_t *video);
int mpeg3video_seek_frame(mpeg3video_t *video, long frame);
int mpeg3video_seek_percentage(mpeg3video_t *video, double percentage);
int mpeg3video_previous_frame(mpeg3video_t *video);
int mpeg3video_drop_frames(mpeg3video_t *video, long frames);
int mpeg3video_read_yuvframe(mpeg3video_t *video,
                                        long frame_number,
                                        char *y_output,
                                        char *u_output,
                                        char *v_output,
                                        int in_x,
                                        int in_y,
                                        int in_w,
                                        int in_h);
int mpeg3video_read_raw(mpeg3video_t *video, unsigned char *output, long *size, long max_size);
int mpeg3video_display_second_field(mpeg3video_t *video);
int mpeg3video_init_output();
int mpeg3video_get_header(mpeg3video_t *video, int dont_repeat);
int mpeg3video_getpicture(mpeg3video_t *video, int framenum);
int mpeg3video_match_refframes(mpeg3video_t *video);
int mpeg3video_next_code(mpeg3_bits_t* stream, unsigned int code);
int mpeg3video_prev_code(mpeg3_bits_t* stream, unsigned int code);
int mpeg3video_getgophdr(mpeg3video_t *video);
int mpeg3video_present_frame(mpeg3video_t *video);
int mpeg3video_read_frame_backend(mpeg3video_t *video, int skip_bframes);
int mpeg3video_getslicehdr(mpeg3_slice_t *slice, mpeg3video_t *video);
int mpeg3video_get_macroblock_address(mpeg3_slice_t *slice);
int mpeg3video_macroblock_modes(mpeg3_slice_t *slice,
                mpeg3video_t *video,
                int *pmb_type,
                int *pstwtype,
                int *pstwclass,
                int *pmotion_type,
                int *pmv_count,
                int *pmv_format,
                int *pdmv,
                int *pmvscale,
                int *pdct_type);
int mpeg3video_motion_vectors(mpeg3_slice_t *slice,
                mpeg3video_t *video,
                int PMV[2][2][2],
                int dmvector[2],
                int mv_field_sel[2][2],
                int s,
                int mv_count,
                int mv_format,
                int h_r_size,
                int v_r_size,
                int dmv,
                int mvscale);
void mpeg3video_motion_vector(mpeg3_slice_t *slice,
                mpeg3video_t *video,
                int *PMV,
                int *dmvector,
                int h_r_size,
                int v_r_size,
                int dmv,
                int mvscale,
                int full_pel_vector);
int mpeg3video_get_cbp(mpeg3_slice_t *slice);
int mpeg3video_clearblock(mpeg3_slice_t *slice, int comp, int size);
int mpeg3video_getmpg2intrablock(mpeg3_slice_t *slice,
                mpeg3video_t *video,
                int comp,
                int dc_dct_pred[]);
int mpeg3video_getintrablock(mpeg3_slice_t *slice,
                mpeg3video_t *video,
                int comp,
                int dc_dct_pred[]);
int mpeg3video_getmpg2interblock(mpeg3_slice_t *slice,
                mpeg3video_t *video,
                int comp);
int mpeg3video_getinterblock(mpeg3_slice_t *slice,
                mpeg3video_t *video,
                int comp);
int mpeg3video_reconstruct(mpeg3video_t *video,
        int bx,
        int by,
        int mb_type,
        int motion_type,
        int PMV[2][2][2],
        int mv_field_sel[2][2],
        int dmvector[2],
        int stwtype);
void mpeg3video_calc_dmv(mpeg3video_t *video,
                int DMV[][2],
                int *dmvector,
                int mvx,
                int mvy);


/* FILESYSTEM */

mpeg3_fs_t* mpeg3_new_fs(char *path);
int mpeg3_delete_fs(mpeg3_fs_t *fs);
int mpeg3io_open_file(mpeg3_fs_t *fs);
int mpeg3io_close_file(mpeg3_fs_t *fs);
int mpeg3io_read_data(unsigned char *buffer, long bytes, mpeg3_fs_t *fs);

/* BITSTREAM */
mpeg3_bits_t* mpeg3bits_new_stream(mpeg3_t *file, mpeg3_demuxer_t *demuxer);
unsigned int mpeg3bits_getbits(mpeg3_bits_t* stream, int n);
int mpeg3bits_read_buffer(mpeg3_bits_t* stream, unsigned char *buffer, int bytes);
int mpeg3bits_use_ptr(mpeg3_bits_t* stream, unsigned char *buffer);
int mpeg3bits_use_demuxer(mpeg3_bits_t* stream);
int mpeg3bits_refill(mpeg3_bits_t* stream);
int mpeg3bits_getbitoffset(mpeg3_bits_t *stream);
void mpeg3bits_start_reverse(mpeg3_bits_t* stream);
void mpeg3bits_start_forward(mpeg3_bits_t* stream);
int mpeg3bits_delete_stream(mpeg3_bits_t* stream);
int mpeg3bits_byte_align(mpeg3_bits_t *stream);
int mpeg3bits_seek_start(mpeg3_bits_t* stream);
int mpeg3bits_seek_time(mpeg3_bits_t* stream, double time_position);
int mpeg3bits_seek_byte(mpeg3_bits_t* stream, long position);
int mpeg3bits_seek_percentage(mpeg3_bits_t* stream, double percentage);
unsigned int mpeg3bits_next_startcode(mpeg3_bits_t* stream);
int mpeg3bits_seek_end(mpeg3_bits_t* stream);

/* MISC */
int mpeg3_read_toc(mpeg3_t *file);
int mpeg3_generate_toc(FILE *output, char *path, int timecode_search, int print_streams);
int mpeg3_mmx_test();
int mpeg3io_seek(mpeg3_fs_t *fs, long byte);
int mpeg3io_seek_relative(mpeg3_fs_t *fs, long bytes);
int mpeg3io_device(char *path, char *device);
int mpeg3_decrypt_packet(mpeg3_css_t *css, unsigned char *sector);
int mpeg3_delete_css(mpeg3_css_t *css);
int mpeg3_get_keys(mpeg3_css_t *css, char *path);
int mpeg3_copy_fs(mpeg3_fs_t *dst, mpeg3_fs_t *src);
int mpeg3_min(int x, int y);
int mpeg3_max(int x, int y);
int mpeg3_new_slice_buffer(mpeg3_slice_buffer_t *slice_buffer);
int mpeg3_expand_slice_buffer(mpeg3_slice_buffer_t *slice_buffer);
int mpeg3_delete_slice_decoder(mpeg3_slice_t *slice);
int mpeg3_new_slice_decoder(mpeg3video_t *video, mpeg3_slice_t *slice);
int mpeg3_delete_slice_buffer(mpeg3_slice_buffer_t *slice_buffer);

#if defined(__cplusplus)
}
#endif

#endif
