#ifndef __ztxt_h
#define __ztxt_h

#include "CExpander.h"
#include <zlib.h>
#include "pdb.h"
/*
 * Stuff common to both Weasel Reader and makeztxt
 *
 * $Id: weasel_common.h,v 1.4 2001/12/18 04:20:44 foxamemnon Exp $
 *
 */

#ifndef _WEASEL_COMMON_H_
#define _WEASEL_COMMON_H_ 1


/* Padding is no good */
#if defined(__GNUC__) && defined(__UNIX__)
#  pragma pack(2)
#endif

/*  The default creator is Weasel Reader 'GPlm'  */
#define GPLM_CREATOR_ID         "GPlm"
/*  Databases of type 'zTXT'  */
#define ZTXT_TYPE_ID            "zTXT"
/*  Size of one database record  */
#define RECORD_SIZE             8192
/*  Allow largest WBIT size for data.  Lower with command line options
    in makeztxt  */
#define MAXWBITS                15
/* Max length for a bookmark/annotation title */
#define MAX_BMRK_LENGTH         20


/*****************************************************
 *   This is the zTXT document header (record #0)    *
 *            ----zTXT version 1.42----              *
 *****************************************************/
typedef struct zTXT_record0Type {
    UInt16        version;                /* zTXT format version              */
    UInt16        numRecords;             /* Number of data (TEXT) records    */
    UInt32        size;                   /* Size in bytes of uncomp. data    */
    UInt16        recordSize;             /* Size of a single data record     */
    UInt16        numBookmarks;           /* Number of bookmarks in DB        */
    UInt16        bookmarkRecord;         /* Record containing bookmarks      */
    UInt16        numAnnotations;         /* Number of annotation records     */
    UInt16        annotationRecord;       /* Record # of annotation index     */
    UInt8         randomAccess;           /* 1 if compressed w/Z_FULL_FLUSH   */
    UInt8         padding[0x20 - 19];     /* Pad to a size of 0x20 bytes      */
} zTXT_record0;

struct zTXTbkmk
{
    UInt32 offset;
    tchar title[MAX_BMRK_LENGTH];
};

#endif


const UInt32 ZTXT_ID = 0x5458547a;

class ztxt : public CExpander, Cpdb
{
    bool bInit;
    UInt32 buffersize;
    UInt32 buffercontent;
    UInt8* expandedtextbuffer;
    UInt8* compressedtextbuffer;
    z_stream zstream;
    size_t bufferpos;
    UInt16 bufferrec;
    zTXT_record0 hdr0;
    size_t currentpos;
    void home();
 public:
    virtual void sizes(unsigned long& _file, unsigned long& _text)
	{
	    _file = file_length;
	    _text = ntohl(hdr0.size);
	}
    virtual bool hasrandomaccess() { return (hdr0.randomAccess != 0); }
    virtual ~ztxt()
	{
	    if (expandedtextbuffer != NULL) delete [] expandedtextbuffer;
	    if (compressedtextbuffer != NULL) delete [] compressedtextbuffer;
	    if (bInit)
	    {
		inflateEnd(&zstream);
	    }
	}
    ztxt();
    virtual int openfile(const char *src);
    virtual int getch();
    virtual unsigned int locate();
    virtual void locate(unsigned int n);
    virtual CList<Bkmk>* getbkmklist();
};

#endif

