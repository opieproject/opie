#include "libmpeg3.h"
#include "mpeg3protos.h"

#include <stdlib.h>

mpeg3_vtrack_t* mpeg3_new_vtrack(mpeg3_t *file, int stream_id, mpeg3_demuxer_t *demuxer)
{
	int result = 0;
	mpeg3_vtrack_t *new_vtrack;
	new_vtrack = (mpeg3_vtrack_t*)calloc(1, sizeof(mpeg3_vtrack_t));
	new_vtrack->demuxer = mpeg3_new_demuxer(file, 0, 1, stream_id);
	if(demuxer) mpeg3demux_copy_titles(new_vtrack->demuxer, demuxer);
	new_vtrack->current_position = 0;

/* Get information about the track here. */
	new_vtrack->video = mpeg3video_new(file, new_vtrack);
	if(!new_vtrack->video)
	{
/* Failed */
		mpeg3_delete_vtrack(file, new_vtrack);
		new_vtrack = 0;
	}
	return new_vtrack;
}

int mpeg3_delete_vtrack(mpeg3_t *file, mpeg3_vtrack_t *vtrack)
{
	if(vtrack->video)
		mpeg3video_delete(vtrack->video);
	if(vtrack->demuxer)
		mpeg3_delete_demuxer(vtrack->demuxer);
	free(vtrack);
}
