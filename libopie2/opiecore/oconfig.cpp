/*
                             This file is part of the Opie Project

                             (C) 2003 Michael Lauer <mickey@tm.informatik.uni-frankfurt.de>
                             Inspired by the config classes from the KDE Project which are
              =.             (C) 1997 Matthias Kalle Dalheimer <kalle@kde.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

/* QT */

#include <qfont.h>
#include <qcolor.h>

/* OPIE */

#include <opie2/oconfig.h>

OConfig::OConfig( const QString &name, Domain domain )
        :Config( name, domain )
{
}

OConfig::~OConfig()
{
}

QColor OConfig::readColorEntry( const QString& key, const QColor* pDefault ) const
{
  QColor aRetColor;
  int nRed = 0, nGreen = 0, nBlue = 0;

  QString aValue = readEntry( key );
  if( !aValue.isEmpty() )
  {
      if ( aValue.at(0) == '#' )
      {
          aRetColor.setNamedColor(aValue);
      }
      else
      {
          bool bOK;

          // find first part (red)
          int nIndex = aValue.find( ',' );

          if( nIndex == -1 )
          {
            // return a sensible default -- Bernd
            if( pDefault )
              aRetColor = *pDefault;
            return aRetColor;
          }

          nRed = aValue.left( nIndex ).toInt( &bOK );

          // find second part (green)
          int nOldIndex = nIndex;
          nIndex = aValue.find( ',', nOldIndex+1 );

          if( nIndex == -1 )
          {
            // return a sensible default -- Bernd
            if( pDefault )
              aRetColor = *pDefault;
            return aRetColor;
          }
          nGreen = aValue.mid( nOldIndex+1,
                               nIndex-nOldIndex-1 ).toInt( &bOK );

          // find third part (blue)
          nBlue = aValue.right( aValue.length()-nIndex-1 ).toInt( &bOK );

          aRetColor.setRgb( nRed, nGreen, nBlue );
        }
    }
  else {

    if( pDefault )
      aRetColor = *pDefault;
  }

  return aRetColor;
}

// FIXME: The whole font handling has to be revised for Opie

QFont OConfig::readFontEntry( const QString& key, const QFont* pDefault ) const
{
  /*
  QFont aRetFont;

  QString aValue = readEntry( key );
  if( !aValue.isNull() ) {
    if ( aValue.contains( ',' ) > 5 ) {
      // KDE3 and upwards entry
      if ( !aRetFont.fromString( aValue ) && pDefault )
        aRetFont = *pDefault;
    }
    else {
      // backward compatibility with older font formats
      // ### remove KDE 3.1 ?
      // find first part (font family)
      int nIndex = aValue.find( ',' );
      if( nIndex == -1 ){
        if( pDefault )
          aRetFont = *pDefault;
        return aRetFont;
      }
      aRetFont.setFamily( aValue.left( nIndex ) );

      // find second part (point size)
      int nOldIndex = nIndex;
      nIndex = aValue.find( ',', nOldIndex+1 );
      if( nIndex == -1 ){
        if( pDefault )
          aRetFont = *pDefault;
        return aRetFont;
      }

      aRetFont.setPointSize( aValue.mid( nOldIndex+1,
                                         nIndex-nOldIndex-1 ).toInt() );

      // find third part (style hint)
      nOldIndex = nIndex;
      nIndex = aValue.find( ',', nOldIndex+1 );

      if( nIndex == -1 ){
        if( pDefault )
          aRetFont = *pDefault;
        return aRetFont;
      }

      aRetFont.setStyleHint( (QFont::StyleHint)aValue.mid( nOldIndex+1, nIndex-nOldIndex-1 ).toUInt() );

      // find fourth part (char set)
      nOldIndex = nIndex;
      nIndex = aValue.find( ',', nOldIndex+1 );

      if( nIndex == -1 ){
        if( pDefault )
          aRetFont = *pDefault;
        return aRetFont;
      }

      QString chStr=aValue.mid( nOldIndex+1,
                                nIndex-nOldIndex-1 );
      // find fifth part (weight)
      nOldIndex = nIndex;
      nIndex = aValue.find( ',', nOldIndex+1 );

      if( nIndex == -1 ){
        if( pDefault )
          aRetFont = *pDefault;
        return aRetFont;
      }

      aRetFont.setWeight( aValue.mid( nOldIndex+1,
                                      nIndex-nOldIndex-1 ).toUInt() );

      // find sixth part (font bits)
      uint nFontBits = aValue.right( aValue.length()-nIndex-1 ).toUInt();

      aRetFont.setItalic( nFontBits & 0x01 );
      aRetFont.setUnderline( nFontBits & 0x02 );
      aRetFont.setStrikeOut( nFontBits & 0x04 );
      aRetFont.setFixedPitch( nFontBits & 0x08 );
      aRetFont.setRawMode( nFontBits & 0x20 );
    }
  }
  else
    {
      if( pDefault )
        aRetFont = *pDefault;
    }
  return aRetFont;
  */
  return QFont("Helvetica",10);
}
