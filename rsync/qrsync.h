/*****
 */

#ifndef QRSYNC_H
#define QRSYNC_H

#include <qstring.h>

class QRsync
{
public:
    static void generateSignature( QString baseFile, QString sigFile );
    static void generateDiff( QString baseFile, QString sigFile, QString diffFile );
    static void applyDiff( QString baseFile, QString diffFile );
};


#endif
