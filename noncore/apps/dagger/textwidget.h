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

#ifndef TEXTWIDGET_H
#define TEXTWIDGET_H

#include <qwidget.h>

#include <swkey.h>
#include <swmodule.h>

class QTextBrowser;

class TextWidget : public QWidget
{
    Q_OBJECT

public:
    TextWidget( QWidget *parent = 0x0, sword::SWModule *module = 0x0, int numVerses = 5,
                const QFont *font = 0x0 );
    ~TextWidget();

    const QString   &getFullKey()    { return m_fullKey; }
    const QString   &getAbbrevKey()  { return m_abbrevKey; }
    QString          getModuleName() { return QString( m_module->Name() ); }
    sword::SWModule *getModule()     { return m_module; }
    QString          getCurrVerse();

    bool isBibleText() const { return m_isBibleText; }

    void prevChapter();
    void prevVerse();
    void setKey( const QString &newKey );
    void nextVerse();
    void nextChapter();

public slots:
    void slotNumVersesChanged( int numVerses );
    void slotFontChanged( const QFont *newFont );
    void slotOptionChanged();

private:
    sword::SWModule *m_module;      // Sword module to display in this widget
    sword::SWKey    *m_key;         // Current module key
    bool             m_isBibleText; // Indicates whether module is a Bible or not

    QTextBrowser    *m_textView;    // Displays module's text
    QString          m_fullKey;     // Contains full key text in format 'key (module)'
    QString          m_abbrevKey;   // Contains abbreviated key text

    int              m_numVerses;   // Number of verses to display at a time for Bible modules

    void setText();

signals:
    void sigRefClicked( const QString &ref );
};

#endif
