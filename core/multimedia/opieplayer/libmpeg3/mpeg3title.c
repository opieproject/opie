#include "mpeg3private.h"
#include "mpeg3protos.h"
#include "mpeg3title.h"


#include <stdlib.h>


mpeg3_title_t* mpeg3_new_title(mpeg3_t *file, char *path)
{
	mpeg3_title_t *title = (mpeg3_title_t*)calloc(1, sizeof(mpeg3_title_t));
	title->fs = mpeg3_new_fs(path);
	title->file = file;
	return title;
}

int mpeg3_delete_title(mpeg3_title_t *title)
{
	mpeg3_delete_fs(title->fs);
	if(title->timecode_table_size)
	{
		free(title->timecode_table);
	}
	free(title);
	return 0;
}


int mpeg3_copy_title(mpeg3_title_t *dst, mpeg3_title_t *src)
{
	int i;

	mpeg3_copy_fs(dst->fs, src->fs);
	dst->total_bytes = src->total_bytes;
	
	if(src->timecode_table_size)
	{
		dst->timecode_table_allocation = src->timecode_table_allocation;
		dst->timecode_table_size = src->timecode_table_size;
		dst->timecode_table = (mpeg3demux_timecode_t*)calloc(1, sizeof(mpeg3demux_timecode_t) * dst->timecode_table_allocation);

		for(i = 0; i < dst->timecode_table_size; i++)
		{
			dst->timecode_table[i] = src->timecode_table[i];
		}
	}
}

int mpeg3_dump_title(mpeg3_title_t *title)
{
	int i;
	
	for(i = 0; i < title->timecode_table_size; i++)
	{
		printf("%f: %d - %d %f %f %d\n", 
			title->timecode_table[i].absolute_start_time, 
			title->timecode_table[i].start_byte, 
			title->timecode_table[i].end_byte, 
			title->timecode_table[i].start_time, 
			title->timecode_table[i].end_time, 
			title->timecode_table[i].program);
	}
}
