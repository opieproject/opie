#ifndef _AVIFMT_HH_
#define _AVIFMT_HH_

/* 4 bytes */
typedef int WORD;
typedef unsigned int DWORD;
/* 1 byte */
typedef char BYTE;


/* flags for use in flags in AVI_avih */
const DWORD AVIF_HASINDEX=0x00000010;	/* index at end of file */
const DWORD AVIF_MUSTUSEINDEX=0x00000020;
const DWORD AVIF_ISINTERLEAVED=0x00000100;
const DWORD AVIF_TRUSTCKTYPE=0x00000800;
const DWORD AVIF_WASCAPTUREFILE=0x00010000;
const DWORD AVIF_COPYRIGHTED=0x00020000;


struct AVI_avih {
  DWORD	us_per_frame;	// frame display rate (or 0L)
  DWORD max_bytes_per_sec;	// max. transfer rate
  DWORD padding;	// pad to multiples of this size;
  // normally 2K.
  DWORD flags;
  DWORD tot_frames;		// # frames in file
  DWORD init_frames;
  DWORD streams;
  DWORD buff_sz;  
  DWORD width;
  DWORD height;
  DWORD reserved[4];
};


struct AVI_strh {
  unsigned char type[4];      /* stream type */
  unsigned char handler[4];
  DWORD flags;
  DWORD priority;
  DWORD init_frames;       /* initial frames (???) */
  DWORD scale;
  DWORD rate;
  DWORD start;
  DWORD length;
  DWORD buff_sz;           /* suggested buffer size */
  DWORD quality;
  DWORD sample_sz;
  /*
  DWORD frame[4];
     XXX 16 bytes ? */
};


struct AVI_strf {       /* == BitMapInfoHeader */
  DWORD sz;
  DWORD width;
  DWORD height;
  DWORD planes_bit_cnt;
  unsigned char compression[4];
  DWORD image_sz;
  DWORD xpels_meter;
  DWORD ypels_meter;
  DWORD num_colors;        /* used colors */
  DWORD imp_colors;        /* important colors */
  /* may be more for some codecs */
};


struct AVI_list_hdr {
  unsigned char id[4];
  DWORD sz;
  unsigned char type[4];
};


struct AVI_list_odml {
  struct AVI_list_hdr list_hdr;

  unsigned char id[4];
  DWORD sz;
  DWORD frames;
};


struct AVI_list_strl {
  struct AVI_list_hdr list_hdr;
  
  /* chunk strh */
  unsigned char strh_id[4];
  DWORD strh_sz;
  struct AVI_strh strh;

  /* chunk strf */
  unsigned char strf_id[4];
  DWORD strf_sz;
  struct AVI_strf strf;

  /* list odml */
  struct AVI_list_odml list_odml;
};


struct AVI_list_hdrl {
  struct AVI_list_hdr list_hdr;

  /* chunk avih */
  unsigned char avih_id[4];
  DWORD avih_sz;
  struct AVI_avih avih;
  
  /* list strl */
  struct AVI_list_strl strl;
};

#endif
