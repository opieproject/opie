/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2004 Holger Hans Peter <freyther@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
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

#include "messagebox.h"

#include <qapplication.h>
#include <qmessagebox.h>

/*
 * LGPLv2 KDE Project kstringhandler.cpp
 */
template<class T>
inline const T& kClamp( const T& x, const T& low, const T& high )
{
    if ( x < low )       return low;
    else if ( high < x ) return high;
    else                 return x;
}

/**
 * dependant on the screen rotation place the dots
 */
static QString g_insert_ldot( const QString& name, const QFontMetrics& fontMetrics ) {
    uint  maxPixels = qApp->desktop()->width()-90;
    uint nameWidth = fontMetrics.width(name);

    if (maxPixels < nameWidth) {
        QString tmp = name;
        const uint em = fontMetrics.maxWidth();
        maxPixels -= fontMetrics.width("...");

        while (maxPixels < nameWidth && !tmp.isEmpty()) {
            int delta = (nameWidth - maxPixels) / em;
            delta = kClamp(delta, 1, delta); // no max

            tmp.remove(0, delta);
            nameWidth = fontMetrics.width(tmp);
        }

        return ("..." + tmp);
    }

    return name;
}

/**
 *
 * #FIXME Write Own message box to be more independant on sizes
 * #FIXME Ask translator how to make the sentence more robust
 *
 * \brief replacement for QPEMessageBox::confirmDelete
 *
 * If you want to delete a file and the path is too long to fit
 * on the screen \ldots is inserted in the middle of the string
 * to fit on the screen. This allows the user still to identify
 * the file.
 *
 * @param parent  The parent of this MessageBox
 * @param type    The type of the object to delte. i.e 'the image'
 * @param object  The 'object' to be deleted
 * @param caption An optional caption for the box
 *
 */
bool OMessageBox::confirmDelete( QWidget* parent, const QString& type, const QString& object,
                                 const QString& _caption ) {
    /*
     * create a messagebox to get the font metrics
     */
    QMessageBox msg( QString::null, QString::null,
                     QMessageBox::Warning, QMessageBox::Yes,
                     QMessageBox::No|QMessageBox::Default|QMessageBox::Escape,
                     QMessageBox::NoButton,
                     parent, "OMessageBox::confirmDelete" );

    /*
     * Create the Message and Caption
     */
    QString msga = QObject::tr("<qt>Are you sure you want to delete %1<br> %2?</qt>" )
                   .arg( type )
                   .arg( g_insert_ldot( object, msg.fontMetrics() ) );
    QString caption = _caption.isEmpty() ?
                      QObject::tr( "Confirm Deletion" ) : _caption;

    msg.setText( msga );
    msg.setCaption( caption );
    msg.setIcon( QMessageBox::Warning );
    msg.adjustSize();

    /*
     * Warn the user that he will delete
     */
    int ret = msg.exec();
    return ( ret == QMessageBox::Yes );
}
