#include <qfile.h>
#include <qtextstream.h>

#include "kdebug.h"

#include "kconfig.h"

QString KConfig::mGroup = "";
//QString KConfig::mGroup = "General";

KConfig::KConfig( const QString &fileName )
  : mFileName( fileName ), mDirty( false )
{
  kdDebug() << "KConfig::KConfig(): '" << fileName << "'" << endl;

  load();
}


KConfig::~KConfig()
{
  sync();
}

void KConfig::setGroup( const QString &group )
{
  return;
  
//  kdDebug() << "KConfig::setGroup(): '" << group << "'" << endl;

  mGroup = group;

  if ( mGroup.right( 1 ) != "/" ) mGroup += "/";
}


QValueList<int> KConfig::readIntListEntry( const QString & )
{
  QValueList<int> l;
  return l;
}

int KConfig::readNumEntry( const QString &, int def )
{
  return def;
}

QString KConfig::readEntry( const QString &key, const QString &def )
{
  QMap<QString,QString>::ConstIterator it = mStringMap.find( mGroup + key );
  
  if ( it == mStringMap.end() ) {
    return def;
  }
  
  return *it;
}

QStringList KConfig::readListEntry( const QString & )
{
  return QStringList();
}

bool KConfig::readBoolEntry( const QString &key, bool def )
{
  QMap<QString,bool>::ConstIterator it = mBoolMap.find( mGroup + key );
  
  if ( it == mBoolMap.end() ) {
    return def;
  }
  
  return *it;
}

QColor KConfig::readColorEntry( const QString &, QColor *def )
{
  if ( def ) return *def;
  return QColor();
}

QFont KConfig::readFontEntry( const QString &, QFont *def )
{
  if ( def ) return *def;
  return QFont();
}


void KConfig::writeEntry( const QString &, QValueList<int> )
{
}

void KConfig::writeEntry( const QString &, int )
{
}

void KConfig::writeEntry( const QString &key, const QString &value )
{
  mStringMap.insert( mGroup + key, value );

  mDirty = true;
}

void KConfig::writeEntry( const QString &, const QStringList & )
{
}

void KConfig::writeEntry( const QString &key, bool value)
{
  mBoolMap.insert( mGroup + key, value );

  mDirty = true;
}

void KConfig::writeEntry( const QString &, const QColor & )
{
}

void KConfig::writeEntry( const QString &, const QFont & )
{
}

void KConfig::load()
{
  mBoolMap.clear();
  mStringMap.clear();

  QFile f( mFileName );
  if ( !f.open( IO_ReadOnly ) ) {
    kdDebug() << "KConfig::load(): Can't open file '" << mFileName << "'"
              << endl;
    return;
  }

  
  QTextStream t( &f );
  
  QString line = t.readLine();

  while ( !line.isNull() ) {
    QStringList tokens = QStringList::split( ",", line );
    if ( tokens[0] == "bool" ) {
      bool value = false;
      if ( tokens[2] == "1" ) value = true;
      
      mBoolMap.insert( tokens[1], value );
    } else if ( tokens[0] == "QString" ) {
      QString value = tokens[2];
      mStringMap.insert( tokens[1], value );
    }
  
    line = t.readLine();
  }
}

void KConfig::sync()
{
  if ( !mDirty ) return;

  QFile f( mFileName );
  if ( !f.open( IO_WriteOnly ) ) {
    kdDebug() << "KConfig::sync(): Can't open file '" << mFileName << "'"
              << endl;
    return;
  }

  QTextStream t( &f );
  
  QMap<QString,bool>::ConstIterator itBool;
  for( itBool = mBoolMap.begin(); itBool != mBoolMap.end(); ++itBool ) {
    t << "bool," << itBool.key() << "," << (*itBool ) << endl;
  }

  QMap<QString,QString>::ConstIterator itString;
  for( itString = mStringMap.begin(); itString != mStringMap.end(); ++itString ) {
    t << "QString," << itString.key() << "," << (*itString ) << endl;
  }

  f.close();

  mDirty = false;
}
