/*

  A simple converter of JPEGs to an AVI-MJPEG animation.

  USAGE: 
  1. jpegtoavi {usec per img} {img width} {img height} 
		{img1 .. imgN}
  (writes AVI file to stdout)

  2. jpegtoavi -f {fps} {img width} {img height}
               {img1 .. imgN}
  (same as #1 but with period specified in terms of fps)

  3. jpegtoavi -fsz {img1 .. imgN}
  (writes sz in bytes of such an AVI file to stdout)
*/

#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE

#include <stdio.h>
#include "avifmt.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "byteswap.h"
#include <stdlib.h>


/*
  spc: indicating file sz in bytes, -1 on error
*/
off_t file_sz(char *fn)
{
  struct stat s;
  if(stat(fn,&s)==-1)
    return -1;
  return s.st_size;
}


/*
  spc: returning sum of sizes of named JPEGs, -1 on error;
      file sizes adjusted to multiple of 4-bytes
  pos: szarray, if non-0, contains true sizes of files
*/
off_t img_array_sz(char **img,int imgsz,DWORD *szarray)
{
  off_t tmp,ret=0;
  int i=0;
  for(;i<imgsz;++i) {
    if((tmp=file_sz(img[i]))==-1)
      return -1;
    if(szarray)
      szarray[i]=(DWORD)tmp;
    tmp+=((4-(tmp%4))%4);
    ret+=tmp;
  }
  return ret;
}


/*
  spc: printing 4 byte word in little-endian fmt
*/
void print_quartet(unsigned int i)
{
  putchar(i%0x100); i/=0x100;
  putchar(i%0x100); i/=0x100;
  putchar(i%0x100); i/=0x100;
  putchar(i%0x100);
}


inline void show_help(int argc,char **argv)
{
  fprintf(stderr,"USAGE: jpegtoavi {usec per img} {img width} {img height} {img1 .. imgN}\n");
  fprintf(stderr,"       jpegtoavi -f {fps} {img width} {img height} {img1 .. imgN}\n");
  fprintf(stderr,"       jpegtoavi --fsz {img1 .. imgN}\n");
}


int main(int argc,char **argv)
{
  DWORD per_usec=1;
  DWORD width;
  DWORD height;
  DWORD frames=1;
  unsigned int fps;
  unsigned short img0;
  off64_t jpg_sz_64,riff_sz_64;
  long jpg_sz=1;
  const off64_t MAX_RIFF_SZ=2147483648LL; /* 2 Gb limit */
  DWORD riff_sz;

  int fd,f;
  long nbr,nbw,tnbw=0;
  char buff[512];
  DWORD *szarray=0;
  DWORD *offsets=0;
  off_t mfsz,remnant;

  struct AVI_list_hdrl hdrl={
    /* header */
    {
      {'L','I','S','T'},
      LILEND4(sizeof(struct AVI_list_hdrl)-8),
      {'h','d','r','l'}
    },

    /* chunk avih */
    {'a','v','i','h'},
    LILEND4(sizeof(struct AVI_avih)),
    {
      LILEND4(per_usec),
      LILEND4(1000000*(jpg_sz/frames)/per_usec),
      LILEND4(0),
      LILEND4(AVIF_HASINDEX),
      LILEND4(frames),
      LILEND4(0),
      LILEND4(1),
      LILEND4(0),
      LILEND4(width),
      LILEND4(height),
      {LILEND4(0),LILEND4(0),LILEND4(0),LILEND4(0)}
    },

    /* list strl */
    {
      {
	{'L','I','S','T'},
	LILEND4(sizeof(struct AVI_list_strl)-8),
	{'s','t','r','l'}
      },

      /* chunk strh */
      {'s','t','r','h'},
      LILEND4(sizeof(struct AVI_strh)),
      {
	{'v','i','d','s'},
	{'M','J','P','G'},
	LILEND4(0),
	LILEND4(0),
	LILEND4(0),
	LILEND4(per_usec),
	LILEND4(1000000),
	LILEND4(0),
	LILEND4(frames),
	LILEND4(0),
	LILEND4(0),
	LILEND4(0)
      },
      
      /* chunk strf */
      {'s','t','r','f'},
      sizeof(struct AVI_strf),
      {      
	LILEND4(sizeof(struct AVI_strf)),
	LILEND4(width),
	LILEND4(height),
	LILEND4(1+24*256*256),
	{'M','J','P','G'},
	LILEND4(width*height*3),
	LILEND4(0),
	LILEND4(0),
	LILEND4(0),
	LILEND4(0)
      },

      /* list odml */
      {
	{
	  {'L','I','S','T'},
	  LILEND4(16),
	  {'o','d','m','l'}
	},
	{'d','m','l','h'},
	LILEND4(4),
	LILEND4(frames)
      }
    }
  };

  /* parsing command line arguments */
  if(argc<2) {
    show_help(argc,argv);
    return -1;
  }
  else if(argc==2 && strcmp(argv[1],"--version")==0) {
    printf("jpegtoavi v%d.%d\n",VERSION_MAJ,VERSION_MIN);
    return 0;
  }
  else if(argc<3) {
    show_help(argc,argv);
    return -1;
  }
  else if(strcmp(argv[1],"-fsz")==0) {
    img0=2;
  }
  else if(strcmp(argv[1],"-f")==0) {
    if(argc<6 || sscanf(argv[2],"%u",&fps)!=1
       || fps==0
       || sscanf(argv[3],"%u",&width)!=1
       || sscanf(argv[4],"%u",&height)!=1) {
      show_help(argc,argv);
      return -1;
    }
    else {
      per_usec=1000000/fps;
      img0=5;
    }
  }
  else if(argc<5 || sscanf(argv[1],"%u",&per_usec)!=1 
     || sscanf(argv[2],"%u",&width)!=1
     || sscanf(argv[3],"%u",&height)!=1) {
    show_help(argc,argv);
    return -1;
  }
  else {
    img0=4;
  }
  frames=argc-img0;

  /* getting image, riff sizes */
  if(img0!=2) {
    if((szarray=(DWORD *)malloc(frames*sizeof(DWORD)))==0) {
      fprintf(stderr,"malloc error");
      free(offsets);
      return -5;
    }
  }
  jpg_sz_64=img_array_sz(&argv[img0],frames,szarray);
  if(jpg_sz_64==-1) {
    fprintf(stderr,"couldn't determine size of images\n");
    return -2;
  }
  riff_sz_64=sizeof(struct AVI_list_hdrl)+4+4+jpg_sz_64
    +8*frames+8+8+16*frames;
  if(riff_sz_64>=MAX_RIFF_SZ) {
    fprintf(stderr,"RIFF would exceed 2 Gb limit\n");
    return -3;
  }
  jpg_sz=(long)jpg_sz_64;
  riff_sz=(DWORD)riff_sz_64;

  /* printing RIFF size and quitting */
  if(img0==2) {
    printf("%lu\n",(unsigned long)riff_sz+8UL);
    return 0;
  }

  /* printing AVI.. riff hdr */
  printf("RIFF");
  print_quartet(riff_sz);
  printf("AVI ");

  /* list hdrl */
  hdrl.avih.us_per_frame=LILEND4(per_usec);
  hdrl.avih.max_bytes_per_sec=LILEND4(1000000*(jpg_sz/frames)
				      /per_usec);
  hdrl.avih.tot_frames=LILEND4(frames);
  hdrl.avih.width=LILEND4(width);
  hdrl.avih.height=LILEND4(height);
  hdrl.strl.strh.scale=LILEND4(per_usec);
  hdrl.strl.strh.rate=LILEND4(1000000);
  hdrl.strl.strh.length=LILEND4(frames);
  hdrl.strl.strf.width=LILEND4(width);
  hdrl.strl.strf.height=LILEND4(height);
  hdrl.strl.strf.image_sz=LILEND4(width*height*3);
  hdrl.strl.list_odml.frames=LILEND4(frames);
  fwrite(&hdrl,sizeof(hdrl),1,stdout);

  /* list movi */
  printf("LIST");
  print_quartet(jpg_sz+8*frames+4);
  printf("movi");

  if((offsets=(DWORD *)malloc(frames*sizeof(DWORD)))==0) {
    fprintf(stderr,"malloc error");
    return -4;
  }

  for(f=img0;f<argc;++f) {
    printf("00db");
    mfsz=szarray[f-img0];
    remnant=(4-(mfsz%4))%4;
    print_quartet(mfsz+remnant);
    szarray[f-img0]+=remnant;
    if(f==img0) {
      offsets[0]=4;
    }
    else {
      offsets[f-img0]=offsets[f-img0-1]+szarray[f-img0-1]+8;
    }
    if((fd=open(argv[f],O_RDONLY))<0) {
      fprintf(stderr,"couldn't open file!\n");
      free(offsets);
      free(szarray);
      return -6;
    }
    nbw=0;

    if((nbr=read(fd,buff,6))!=6) {
      fprintf(stderr,"error\n");
      free(offsets);
      free(szarray);
      return -7;
    }
    fwrite(buff,nbr,1,stdout);
    read(fd,buff,4);
    fwrite("AVI1",4,1,stdout);
    nbw=10;

    while((nbr=read(fd,buff,512))>0) {
      fwrite(buff,nbr,1,stdout);
      nbw+=nbr;
    }
    if(remnant>0) {
      fwrite(buff,remnant,1,stdout);
      nbw+=remnant;
    }
    tnbw+=nbw;
    close(fd);
  }
  if(tnbw!=jpg_sz) {
    fprintf(stderr,"error writing images (wrote %ld bytes, expected %ld bytes)\n",
	    tnbw,jpg_sz);
    free(offsets);
    free(szarray);
    return -8;
  }
  
  // indices
  printf("idx1");
  print_quartet(16*frames);
  for(f=0;f<frames;++f) {
    printf("00db");
    print_quartet(18);
    print_quartet(offsets[f]);
    print_quartet(szarray[f]);
  }

  free(offsets);
  free(szarray);
  return 0;
}
  
