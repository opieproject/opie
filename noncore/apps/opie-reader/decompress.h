#include "mytypes.h"

size_t UnZip(UInt8* compressedbuffer,size_t reclen,UInt8* tgtbuffer,size_t bsize);

size_t (*getdecompressor(char* _s))(UInt8*, size_t, UInt8*, size_t);
