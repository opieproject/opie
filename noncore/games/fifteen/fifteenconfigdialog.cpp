/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 <>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
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
  -_. . .   )=.  =           General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/


#include "fifteenconfigdialog.h"

#include <opie2/ofiledialog.h>

#include <qimage.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qgroupbox.h>


using Opie::Ui::OFileSelector;
using Opie::Ui::OFileDialog;


FifteenConfigDialog::FifteenConfigDialog( QWidget* parent, const char* name, bool modal )
    : FifteenConfigDialogBase( parent, name, modal )
{
    grpGameGrid->hide();
}

FifteenConfigDialog::~FifteenConfigDialog()
{}

/**
 * src.isEmpty() means no Custom Image to be set
 */
void FifteenConfigDialog::setImageSrc( const QString& src ) {
    ckbCustomImage->setChecked( !src.isEmpty() );
    lneImage->setText( src );
    lblPreview->setPixmap( preview(src ) );
}

/*
 * If the return isEmpty() this means no custom image is wished
 */
QString FifteenConfigDialog::imageSrc()const {
    return ckbCustomImage->isChecked() ? lneImage->text() : QString::null;
}

void FifteenConfigDialog::setGameboard( int rows, int columns ) {
    spnRow->setValue( rows );
    spnCol->setValue( columns );
}


int FifteenConfigDialog::columns()const {
    return spnCol->value();
}

int FifteenConfigDialog::rows() const{
    return spnRow->value();
}

void FifteenConfigDialog::slotLoadImage() {
    QStringList lst;
    lst << "image/*";
    MimeTypes type;
    type.insert( tr("All Images" ), lst );
    type.insert( tr("All Files"), "*/*" );


    QString str = OFileDialog::getOpenFileName(OFileSelector::Normal,
                                               QString::null, QString::null,
                                               type, this,
                                               tr("Select board background") );
    if (!str.isEmpty() )
        setImageSrc( str );
}


QPixmap FifteenConfigDialog::preview( const QString& file ) {
    QPixmap pix;
    QImage img( file );
    if( img.isNull() )
        return pix;

    img = img.smoothScale(120, 120 );
    pix.convertFromImage( img );

    return pix;
}
