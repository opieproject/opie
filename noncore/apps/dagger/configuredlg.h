/*
Dagger - A Bible study program utilizing the Sword library.
Copyright (c) 2004 Dan Williams <drw@handhelds.org>

This file is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later version.

This file is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this
file; see the file COPYING. If not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef CONFIGUREDLG_H
#define CONFIGUREDLG_H

#include <opie2/ofontselector.h>
#include <opie2/otabwidget.h>

#include <qcheckbox.h>
#include <qdialog.h>
#include <qlineedit.h>
#include <qradiobutton.h>
#include <qspinbox.h>

class QLabel;

class ConfigureDlg : public QDialog
{
    Q_OBJECT

public:
    ConfigureDlg( QWidget *parent = 0x0, const QString &swordPath = 0x0, bool alwaysOpenNew = true,
                  int numVerses = 5, bool disableBlanking = false, int copyFormat = 0, const QFont *font = 0x0 );

    QString swordPath()     { return m_swordPath->text(); }
    bool    alwaysOpenNew() { return m_alwaysOpenNew->isChecked(); }
    int     numVerses()     { return m_numVerses->value(); }
    bool    screenBlank()   { return m_disableScreenBlank->isChecked(); }
    int     copyFormat()    { if ( m_copyFull->isChecked() )
                                  return 1;
                              else if ( m_copyVerse->isChecked() )
                                  return 2;
                              else if ( m_copyKey->isChecked() )
                                  return 3;
                              else
                                  return 0; }
    QFont   selectedFont()  { return m_font->selectedFont(); }

private:
    Opie::Ui::OTabWidget     m_tabs; // Main widget
    Opie::Ui::OFontSelector *m_font; // Font selection widget

    // General tab's UI controls
    QLineEdit *m_swordPath;          // Contains path to Sword modules
    QCheckBox *m_alwaysOpenNew;      // Indicates whether to always open modules in a new tab
    QSpinBox  *m_numVerses;          // Contains number of verses to display for Bible modules
    QCheckBox *m_disableScreenBlank; // Indicates whether to disable automatic screen blanking

    // Copy tab's UI controls
    QRadioButton *m_copyTextFull;    // Verse (Text, Book cc:vv)
    QRadioButton *m_copyFull;        // Verse (Book cc:vv)
    QRadioButton *m_copyVerse;       // Verse
    QRadioButton *m_copyKey;         // Book cc:vv
    QLabel       *m_copyExample;     // Text of copy format example

private slots:
    void slotCopyFormatSelected();
};

#endif
