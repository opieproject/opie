/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 Robert Griebl <sandman@handhelds.org>
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
