/***************************************************************************
 *                                                                         *
 *   DrawPad - a drawing program for Opie Environment                      *
 *                                                                         *
 *   (C) 2002 by S. Prud'homme <prudhomme@laposte.net>                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NEWPAGEDIALOG_H
#define NEWPAGEDIALOG_H

#include <qdialog.h>

class QSpinBox;

class NewPageDialog : public QDialog
{ 
    Q_OBJECT

public:
    NewPageDialog(QWidget* parent = 0, const char* name = 0);
    ~NewPageDialog();

    void setWidth(int width);
    void setHeight(int height);

    int width();
    int height();

private:
    QSpinBox* m_pWidthSpinBox;
    QSpinBox* m_pHeightSpinBox;
};

#endif // NEWPAGEDIALOG_H
