/*
 * GPLv2 only
 *
 * TT 2002-2002,2003
 */
#include <qfile.h>
#include <qtextstream.h>

#include <qtopia/mimetype.h>

#include "launcherglobal.h"

bool Opie::Global::isAppLnkFileName( const QString& file ) {
    if ( file.right(1) == "/" )
	return FALSE;

    return file.find(MimeType::appsFolderName()+"/")==0;
}

QString Opie::Global::tempDir() {
    return QString::fromLatin1("/tmp/");
}

Global::Command* Opie::Global::builtinCommands() {
    return builtin;
}

QGuardedPtr<QWidget>* Opie::Global::builtinRunning() {
    return running;
}

QString Opie::Global::uuid() {
    QFile file( "/proc/sys/kernel/random/uuid" );
    if (!file.open(IO_ReadOnly ) )
        return QString::null;

    QTextStream stream(&file);

    return "{" + stream.read().stripWhiteSpace() + "}";
}

QByteArray Opie::Global::encodeBase64(const QByteArray& origData ) {
// follows simple algorithm from rsync code
    uchar *in = (uchar*)origData.data();

    int inbytes = origData.size();
    int outbytes = ((inbytes * 8) + 5) / 6;
    int padding = 4-outbytes%4; if ( padding == 4 ) padding = 0;

    QByteArray outbuf(outbytes+padding);
    uchar* out = (uchar*)outbuf.data();

    const char *b64 =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    for (int i = 0; i < outbytes; i++) {
        int byte = (i * 6) / 8;
        int bit = (i * 6) % 8;
        if (bit < 3) {
            if (byte < inbytes)
	        *out = (b64[(in[byte] >> (2 - bit)) & 0x3F]);
        } else {
            if (byte + 1 == inbytes) {
	        *out = (b64[(in[byte] << (bit - 2)) & 0x3F]);
            } else {
	        *out = (b64[(in[byte] << (bit - 2) |
		     in[byte + 1] >> (10 - bit)) & 0x3F]);
            }
        }
        ++out;
    }
    ASSERT(out == (uchar*)outbuf.data() + outbuf.size() - padding);
    while ( padding-- )
        *out++='=';

    return outbuf;
}


