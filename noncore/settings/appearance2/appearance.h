/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
***********************************************************************
**
**  Enhancements by: Dan Williams, <williamsdr@acm.org>
**
**********************************************************************/

#ifndef APPEARANCESETTINGS_H
#define APPEARANCESETTINGS_H

#include <qpe/fontdatabase.h>

#include <qmainwindow.h>
#include <qdialog.h>

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QListBox;
class QMultiLineEdit;
class QPushButton;
class QRadioButton;
class QToolButton;
class SampleWindow;
class OFontSelector;

class Appearance : public QDialog
{
    Q_OBJECT

public:
    Appearance( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~Appearance();

protected:
	virtual void accept ( );
	virtual void done ( int r );

protected slots:
    void styleClicked ( int );
    void styleSettingsClicked ( );
    void decoClicked ( int );
    void fontClicked ( const QFont & );
    void colorClicked ( int );

    void editSchemeClicked();
    void saveSchemeClicked();
    void deleteSchemeClicked();

private:
	void loadStyles ( QListBox * );
	void loadDecos ( QListBox * );
    void loadColors ( QListBox * );

    void changeText();
	
	QWidget *createStyleTab ( QWidget *parent );
	QWidget *createDecoTab ( QWidget *parent );
	QWidget *createFontTab ( QWidget *parent );
	QWidget *createColorTab ( QWidget *parent );
	QWidget *createGuiTab ( QWidget *parent );

private:
    bool m_style_changed;
    bool m_font_changed;
    bool m_scheme_changed;
    bool m_deco_changed;
    bool m_color_changed;

    int  m_original_style;
    int  m_original_deco;
    int  m_original_tabstyle;
    bool  m_original_tabpos;
    
    QListBox *    m_style_list;
    QPushButton * m_style_settings;

    QListBox *    m_deco_list;

    QListBox *    m_color_list;

	OFontSelector *m_fontselect;

	SampleWindow *m_sample;
    
    QComboBox *   m_tabstyle_list;
    QRadioButton *m_tabstyle_top;
    QRadioButton *m_tabstyle_bottom;
};

#endif
