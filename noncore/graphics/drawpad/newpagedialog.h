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

class QButtonGroup;
class QSpinBox;

class NewPageDialog : public QDialog
{ 
    Q_OBJECT

public:
    NewPageDialog(uint width, uint height, const QColor& foregroundColor,
                  const QColor& backgroundColor, QWidget* parent = 0, const char* name = 0);
    ~NewPageDialog();

    uint selectedWidth();
    uint selectedHeight();
    const QColor& selectedColor();

private:
    QSpinBox* m_pWidthSpinBox;
    QSpinBox* m_pHeightSpinBox;

    QColor m_penColor;
    QColor m_brushColor;

    QButtonGroup* m_pContentButtonGroup;
};

#endif // NEWPAGEDIALOG_H
