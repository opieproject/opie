/*
                             This file is part of the Opie Project

                             (C) 2003 Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
              =.
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

#ifndef ODIALOG_H
#define ODIALOG_H

class QLayoutItem;

#include <qdialog.h>

/**
 * Dialog with extended nonmodal support and methods for OPIE standard
 * compliance.
 *
 * The @ref marginHint() and @ref spacingHint() sizes shall be used
 * whenever you layout the interior of a dialog. One special note. If
 * you make your own action buttons (OK, Cancel etc), the space
 * beteween the buttons shall be @ref spacingHint(), whereas the space
 * above, below, to the right and to the left shall be @ref marginHint().
 * If you add a separator line above the buttons, there shall be a
 * @ref marginHint() between the buttons and the separator and a
 * @ref marginHint() above the separator as well.
 *
 * @author Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
 */

class ODialog : public QDialog
{
  Q_OBJECT

  public:

    /**
     * Constructor.
     *
     * Takes the same arguments as @ref QDialog.
     */
    ODialog(QWidget *parent = 0, const char *name = 0,
	    bool modal = false, WFlags f = 0);

    /**
     * Return the number of pixels you shall use between a
     * dialog edge and the outermost widget(s) according to the KDE standard.
     **/
    static int marginHint();

    /**
     * Return the number of pixels you shall use between
     * widgets inside a dialog according to the KDE standard.
     */
    static int spacingHint();

  private:
    static int mMarginSize;
    static int mSpacingSize;

    //class ODialogPrivate;
    //ODialogPrivate *d;

};
#endif // ODIALOG_H
