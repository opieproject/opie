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

#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <qdialog.h>

#include <qstrlist.h>

class QComboBox;
class QLineEdit;
class QSpinBox;

class ExportDialog : public QDialog
{ 
    Q_OBJECT

public:
    ExportDialog(uint pageAt, uint pageCount, QWidget* parent = 0, const char* name = 0);
    ~ExportDialog();

    uint selectedFromPage();
    uint selectedToPage();

    QString selectedName();
    QString selectedFormat();

public slots:
    void accept();

private slots:
    void selectionChanged(int id);
    void fromPageChanged(int value);
    void toPageChanged(int value);

private:
    uint m_pageAt;
    uint m_pageCount;

    QSpinBox* m_pFromPageSpinBox;
    QSpinBox* m_pToPageSpinBox;

    QLineEdit* m_pNameLineEdit;
    QComboBox* m_pFormatComboBox;
};

#endif // EXPORTDIALOG_H
