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

#include "currency.h"
#include "json.h"

#include <qfile.h>
#include <qtextstream.h>
#include <opie2/odebug.h>

CurrencyConverter::CurrencyConverter()
{
}

void CurrencyConverter::loadRates( const QString &filename )
{
    m_timestamp = QDateTime::currentDateTime();
    m_rates.clear();
    QFile f( filename );
    if ( f.open(IO_ReadOnly) ) {
        QTextStream t( &f );
        QString s = t.read();
        json_value *data = json_parse(s.latin1());
        if( data ) {
            if( data->type == json_object ) {
                for (uint i = 0; i < data->u.object.length; i++) {
                    QString name(data->u.object.values[i].name);
                    if( name == "rates" ) {
                        json_value *ratedata = data->u.object.values[i].value;
                        if( ratedata->type == json_object ) {
                            for (uint k = 0; k < ratedata->u.object.length; k++) {
                                QString currname(ratedata->u.object.values[k].name);
                                json_value *ratevalue = ratedata->u.object.values[k].value;
                                if( ratevalue->type == json_double) {
                                    m_rates.insert(currname, ratevalue->u.dbl);
                                }
                            }
                        }
                    }
                    else if( name == "timestamp" ) {
                        json_value *valuedata = data->u.object.values[i].value;
                        if( valuedata->type == json_integer ) {
                            m_timestamp.setTime_t( valuedata->u.integer );
                        }
                    }
                    else if( name == "base" ) {
                        json_value *valuedata = data->u.object.values[i].value;
                        if( valuedata->type == json_string)
                            m_rates[QString::fromUtf8(valuedata->u.string.ptr)] = 1.0;
                    }
                    else {
                        json_value *valuedata = data->u.object.values[i].value;
                        if( valuedata->type == json_string)
                            m_extra[name] = QString::fromUtf8(valuedata->u.string.ptr);
                    }
                }
            }
            json_value_free( data );
        }
    }
}

void CurrencyConverter::loadCurrencies( const QString &filename )
{
    m_currencies.clear();
    QFile f( filename );
    if ( f.open(IO_ReadOnly) ) {
        QTextStream t( &f );
        QString s = t.read();
        json_value *data = json_parse(s.latin1());
        if( data ) {
            if( data->type == json_object ) {
                for (uint i = 0; i < data->u.object.length; i++) {
                    QString name(data->u.object.values[i].name);
                    json_value *valuedata = data->u.object.values[i].value;
                    if( valuedata->type == json_string) {
                        m_currencies.insert(name, QString::fromUtf8(valuedata->u.string.ptr));
                    }
                }
            }
            json_value_free( data );
        }
    }
}

double CurrencyConverter::convertRate(const QString &rateFrom, const QString &rateTo, double value)
{
    double baseValue = value / m_rates[rateFrom];
    return baseValue * m_rates[rateTo];
}

QStringList CurrencyConverter::rateCodes()
{
    QStringList codes;
    QMap<QString,double>::Iterator it;
    for( it = m_rates.begin(); it != m_rates.end(); ++it )
        codes += it.key();
    return codes;
}

QString CurrencyConverter::currencyName( const QString &code )
{
    return m_currencies[code];
}

QDateTime CurrencyConverter::timestamp()
{
    return m_timestamp;
}

bool CurrencyConverter::dataLoaded()
{
    return m_rates.count() > 0;
}

QMap<QString,QString> CurrencyConverter::extraInfo()
{
    return m_extra;
}
