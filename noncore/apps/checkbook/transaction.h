/*
                             This file is part of the OPIE Project
               =.
             .=l.            Copyright (c)  2002 Dan Williams <williamsdr@acm.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <qdialog.h>

class DateBookMonth;
class QComboBox;
class QLineEdit;
class QMultiLineEdit;
class QPushButton;
class QRadioButton;
class QString;
class QWidget;
class TranInfo;

class Transaction : public QDialog
{
	Q_OBJECT

	public:
		Transaction( QWidget * = 0x0, const QString & = 0x0, TranInfo * = 0x0,
					 char = '$' );
		~Transaction();

	private:
		TranInfo *tran;

		char currencySymbol;

		QRadioButton   *withBtn;
		QRadioButton   *depBtn;
		QPushButton    *dateBtn;
		DateBookMonth  *datePicker;
		QLineEdit      *numEdit;
		QLineEdit      *descEdit;
		QComboBox      *catList;
		QComboBox      *typeList;
		QLineEdit      *amtEdit;
		QLineEdit      *feeEdit;
		QMultiLineEdit *noteEdit;

	protected slots:
		void accept();

	private slots:
		void slotWithdrawalClicked();
		void slotDepositClicked();
		void slotDateChanged( int, int, int );
};

#endif
