#include "CBuffer.h"

CBufferBase& CBufferBase::assign(const void* sztmp, size_t ms)
{
    if (ms*membersize > len)
    {
	delete [] buffer;
	buffer = new unsigned char[len = ms*membersize];
    }
    memcpy(buffer, sztmp, ms*membersize);
    return *this;
}

CBufferBase::CBufferBase(size_t ms, size_t n) : len(n), membersize(ms)
{
    buffer = new unsigned char[len*membersize];
    memset(buffer, 0, len*membersize);
}

void* CBufferBase::operator[](int i)
{
    if ((i+1)*membersize > len)
    {
	unsigned char* oldbuffer = buffer;
	buffer = new unsigned char[(i+1)*membersize];
	memcpy(buffer, oldbuffer, len);
	memset(buffer+len, 0, (i+1)*membersize-len);
	len = (i+1)*membersize;
	delete [] oldbuffer;
    }
    return buffer+i*membersize;
}

size_t CBufferBase::bstrlen(unsigned char* _buffer)
{
    if (_buffer == NULL) _buffer = buffer;
    unsigned char* zero = new unsigned char[membersize];
    memset(zero,0,membersize);
    unsigned char* element = _buffer;
    while (memcmp(element, zero, membersize) != 0)
    {
	element += membersize;
    }
    delete [] zero;
    return (element - _buffer)/membersize;
}
