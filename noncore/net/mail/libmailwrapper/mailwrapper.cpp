#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <qdir.h>

#include "mailwrapper.h"
//#include "logindialog.h"
//#include "defines.h"

#define UNDEFINED 64
#define MAXLINE  76
#define UTF16MASK       0x03FFUL
#define UTF16SHIFT      10
#define UTF16BASE       0x10000UL
#define UTF16HIGHSTART  0xD800UL
#define UTF16HIGHEND    0xDBFFUL
#define UTF16LOSTART    0xDC00UL
#define UTF16LOEND      0xDFFFUL


Attachment::Attachment( DocLnk lnk )
{
    doc = lnk;
    size = QFileInfo( doc.file() ).size();
}

Folder::Folder(const QString&tmp_name, const QString&sep  )
{
    name = tmp_name;
    nameDisplay = name;
    separator = sep;
    prefix = "";
}

Folder::~Folder()
{
}

const QString& Folder::Separator()const
{
    return separator;
}

IMAPFolder::IMAPFolder(const QString&name,const QString&sep, bool select,bool no_inf, const QString&aprefix )
    : Folder( name,sep ),m_MaySelect(select),m_NoInferior(no_inf)
{
  // Decode IMAP foldername
  nameDisplay = IMAPFolder::decodeFolderName( name );
  /*
  qDebug( "folder " + name + " - displayed as " + nameDisplay );
  */
    prefix = aprefix;

    if (prefix.length()>0) {
        if (nameDisplay.startsWith(prefix) && nameDisplay.length()>prefix.length()) {
            nameDisplay=nameDisplay.right(nameDisplay.length()-prefix.length());
        }
    }
}

IMAPFolder::~IMAPFolder()
{
}

static unsigned char base64chars[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+,";

/**
 * Decodes base64 encoded parts of the imapfolder name
 * Code taken from kde cvs: kdebase/kioslave/imap4/rfcdecoder.cc
 */
QString IMAPFolder::decodeFolderName( const QString &name )
{
  unsigned char c, i, bitcount;
  unsigned long ucs4, utf16, bitbuf;
  unsigned char base64[256], utf8[6];
  unsigned long srcPtr = 0;
  QCString dst = "";
  QCString src = name.ascii();

  /* initialize modified base64 decoding table */
  memset(base64, UNDEFINED, sizeof(base64));
  for (i = 0; i < sizeof(base64chars); ++i) {
    base64[(int)base64chars[i]] = i;
  }

  /* loop until end of string */
  while (srcPtr < src.length ()) {
    c = src[srcPtr++];
    /* deal with literal characters and &- */
    if (c != '&' || src[srcPtr] == '-') {
      /* encode literally */
      dst += c;
      /* skip over the '-' if this is an &- sequence */
      if (c == '&')
        srcPtr++;
    } else {
      /* convert modified UTF-7 -> UTF-16 -> UCS-4 -> UTF-8 -> HEX */
      bitbuf = 0;
      bitcount = 0;
      ucs4 = 0;
      while ((c = base64[(unsigned char) src[srcPtr]]) != UNDEFINED) {
        ++srcPtr;
        bitbuf = (bitbuf << 6) | c;
        bitcount += 6;
        /* enough bits for a UTF-16 character? */
        if (bitcount >= 16) {
          bitcount -= 16;
          utf16 = (bitcount ? bitbuf >> bitcount : bitbuf) & 0xffff;
          /* convert UTF16 to UCS4 */
          if (utf16 >= UTF16HIGHSTART && utf16 <= UTF16HIGHEND) {
            ucs4 = (utf16 - UTF16HIGHSTART) << UTF16SHIFT;
            continue;
          } else if (utf16 >= UTF16LOSTART && utf16 <= UTF16LOEND) {
            ucs4 += utf16 - UTF16LOSTART + UTF16BASE;
          } else {
            ucs4 = utf16;
          }
          /* convert UTF-16 range of UCS4 to UTF-8 */
          if (ucs4 <= 0x7fUL) {
            utf8[0] = ucs4;
            i = 1;
          } else if (ucs4 <= 0x7ffUL) {
            utf8[0] = 0xc0 | (ucs4 >> 6);
            utf8[1] = 0x80 | (ucs4 & 0x3f);
            i = 2;
          } else if (ucs4 <= 0xffffUL) {
            utf8[0] = 0xe0 | (ucs4 >> 12);
            utf8[1] = 0x80 | ((ucs4 >> 6) & 0x3f);
            utf8[2] = 0x80 | (ucs4 & 0x3f);
            i = 3;
          } else {
            utf8[0] = 0xf0 | (ucs4 >> 18);
            utf8[1] = 0x80 | ((ucs4 >> 12) & 0x3f);
            utf8[2] = 0x80 | ((ucs4 >> 6) & 0x3f);
            utf8[3] = 0x80 | (ucs4 & 0x3f);
            i = 4;
          }
          /* copy it */
          for (c = 0; c < i; ++c) {
            dst += utf8[c];
          }
        }
      }
      /* skip over trailing '-' in modified UTF-7 encoding */
      if (src[srcPtr] == '-')
        ++srcPtr;
    }
  }

  return QString::fromUtf8( dst.data() );
}

Mail::Mail()
    :Opie::oref_count(),name(""), mail(""), to(""), cc(""), bcc(""), reply(""), subject(""), message("")
{
}

MHFolder::MHFolder(const QString&disp_name,const QString&mbox)
    : Folder( disp_name,"/" )
{
    separator = "/";
    name = mbox;
    if (!disp_name.startsWith("/") && disp_name.length()>0)
        name+="/";
    name+=disp_name;
    if (disp_name.length()==0) {
        nameDisplay = separator;
    }
    prefix = mbox;
}

MHFolder::~MHFolder()
{
}
