/***************************************************************************
                          progressdlg.h  -  description
                             -------------------
    begin                : Mon Aug 26 2002
    copyright            : (C) 2002 by Andy Qua
    email                : andy.qua@blueyonder.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/#ifndef PROGRESSDLG_H
#define PROGRESSDLG_H

#include <qdialog.h>

class QLabel;

class ProgressDlg : public QDialog
{ 
    Q_OBJECT

public:
    ProgressDlg( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~ProgressDlg();

    QLabel* TextLabel2;                

    void setText( QString &text );
    void setText( const char *text );
};

#endif // PROGRESSDLG_H
