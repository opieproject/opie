/*
                             This file is part of the Opie Project

              =.             Copyright (C) 2012 Paul Eggleton <bluelightning@bluelightning.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           General Public License along with
    --        :-=`           this file; see the file COPYING.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#ifndef CURRENCY_H
#define CURRENCY_H

#include <qmap.h>
#include <qstringlist.h>
#include <qdatetime.h>

class CurrencyConverter {
public:
    CurrencyConverter();

    QStringList rateCodes();
    double convertRate(const QString &rateFrom, const QString &rateTo, double value);
    void loadCurrencies( const QString &filename );
    void loadRates( const QString &filename );
    QString currencyName( const QString &code );
    QDateTime timestamp();
    bool dataLoaded();
    QMap<QString,QString> extraInfo();

protected:
    QMap<QString,double> m_rates;
    QMap<QString,QString> m_currencies;
    QMap<QString,QString> m_extra;
    QDateTime m_timestamp;
};

#endif
