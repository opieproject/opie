#include <string.h>
#include "decompress.h"
#include <zlib.h>
#include <stdlib.h>

size_t UnZip(UInt8* compressedbuffer, size_t reclen, UInt8* tgtbuffer, size_t bsize)
{
  z_stream zstream;
  memset(&zstream,sizeof(zstream),0);
  zstream.next_in = compressedbuffer;
  zstream.next_out = tgtbuffer;
  zstream.avail_out = bsize;
  zstream.avail_in = reclen;

  zstream.zalloc = Z_NULL;
  zstream.zfree = Z_NULL;
  zstream.opaque = Z_NULL;
  
  //  printf("Initialising\n");
  
  inflateInit(&zstream);
  int err = 0;
  do {
    zstream.next_out  = tgtbuffer;
    zstream.avail_out = bsize;

    err = inflate( &zstream, Z_SYNC_FLUSH );

    //	//qDebug("err:%d - %u", err, zstream.avail_in);

  } while ( err == Z_OK );

  inflateEnd(&zstream);
  return zstream.total_out;
}

#if defined(__STATIC) && defined(USENEF)
#include "Model.h"
size_t (*getdecompressor(char* _s))(UInt8*, size_t, UInt8*, size_t)
{
  if (strcmp(_s, "PluckerDecompress3") == 0)
    {
      return PluckerDecompress3;
    }
  if (strcmp(_s, "PluckerDecompress4") == 0)
    {
      return PluckerDecompress4;
    }
  if (strcmp(_s, "RebDecompress") == 0)
    {
      return RebDecompress;
    }
  return NULL;
}
#else

#include "qfileinfo.h"

#include <dlfcn.h>

size_t (*getdecompressor(char* _s))(UInt8*, size_t, UInt8*, size_t)
{
#ifdef USEQPE
#ifdef OPIE
  QString codecpath(getenv("OPIEDIR"));
#else
  QString codecpath(getenv("QTDIR"));
#endif
  codecpath += "/plugins/reader/support/libpluckerdecompress.so";
#else
  QString codecpath(getenv("READERDIR"));
  codecpath += "/support/libpluckerdecompress.so";
#endif
  qDebug("Codec:%s", (const char*)codecpath);
  if (QFile::exists(codecpath))
    {
      qDebug("Codec:%s", (const char*)codecpath);
      void* handle = dlopen(codecpath, RTLD_LAZY);
      if (handle == 0)
	{
	  qDebug("Can't find codec:%s", dlerror());
	  return NULL;
	}
      return (size_t (*)(UInt8*, size_t, UInt8*, size_t))dlsym(handle, _s);
    }
  return NULL;
}
#endif
