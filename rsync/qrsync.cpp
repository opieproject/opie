#include "qrsync.h"
#include <stdio.h>
extern "C" {
#include "rsync.h"
}
#include <stdlib.h>

#include <qfile.h>

static const char *rdiffNewFile = "/tmp/rdiff/result";
static size_t block_len = RS_DEFAULT_BLOCK_LEN;
static size_t strong_len = RS_DEFAULT_STRONG_LEN;


void QRsync::generateSignature( QString baseFile, QString sigFile )
{
    
    if ( QFile::exists( baseFile ) ) {
	FILE *basis_file, *sig_file;
	rs_result       result;
	    
	basis_file = fopen( baseFile.latin1(), "rb" );
	sig_file = fopen( sigFile.latin1(), "wb" );

	result = rs_sig_file(basis_file, sig_file, block_len, strong_len, 0);

	fclose( basis_file );
	fclose( sig_file );
	if (result != RS_DONE)
	    printf("error in rdiffGenSig: %d", result );
    }
}


void QRsync::generateDiff( QString baseFile, QString sigFile, QString deltaFile )
{
    if ( QFile::exists( baseFile ) && QFile::exists( sigFile ) ) {
	FILE            *sig_file, *new_file, *delta_file;
	rs_result       result;
	rs_signature_t  *sumset;

	sig_file = fopen(sigFile.latin1(), "rb");
	new_file = fopen(baseFile.latin1(), "rb");
	delta_file = fopen(deltaFile.latin1(), "wb");

	result = rs_loadsig_file(sig_file, &sumset, 0);
	if (result != RS_DONE) {
	    qDebug( "rdiffGenDiff: loading of sig file failed, error=%d", result );
	} else {
	    result = rs_build_hash_table(sumset);
	    if ( result != RS_DONE) {
		qDebug( "rdiffGenDiff: building of hash table failed, error=%d", result );
	    } else {
		result = rs_delta_file(sumset, new_file, delta_file, 0);
		if ( result != RS_DONE) {
		    qDebug( "rdiffGenDiff: writing of diff file failed, error=%d", result );
		}
	    }
	}
	
	if ( sumset )
	    rs_free_sumset( sumset );
	fclose( new_file );
	fclose( delta_file );
	fclose( sig_file );

    }
}

void QRsync::applyDiff( QString baseFile, QString deltaFile )
{
    if ( QFile::exists( baseFile ) && QFile::exists( deltaFile ) ) {
	FILE               *basis_file, *delta_file, *new_file;
	rs_result           result;

	basis_file = fopen(baseFile.latin1(), "rb");
	delta_file = fopen(deltaFile.latin1(), "rb");
#ifdef Q_WS_WIN
	new_file = fopen( (baseFile + ".new").latin1(), "wb" );
#else
	new_file =   fopen(rdiffNewFile, "wb");
#endif	

	result = rs_patch_file(basis_file, delta_file, new_file, 0);

	fclose( basis_file );
	fclose( delta_file );
	fclose( new_file );

	if (result != RS_DONE) {
	    qDebug( "rdiffApplyDiff failed with result %d", result );
	    return;
	}


#ifdef Q_WS_WIN	
	QDir dir;
	QFile backup = baseFile + "~";
	dir.rename( baseFile, backup );
	dir.rename( (baseFile + ".new"), baseFile );
	dir.remove( backup );
#else
	QString cmd = "mv ";
	cmd += rdiffNewFile;
	cmd += " " + baseFile;
	system( cmd.latin1() );
#endif
    }

}
