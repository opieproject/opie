/*
               =.            This file is part of the Opie Project
             .=l.            Copyright (C) 2004 Opie Developer Team <opie-devel@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

/*
 * Opie Sheet (formerly Sheet/Qt)
 * by Serdar Ozler <sozler@sitebest.com>
 */

#ifndef CELLFORMAT_H
#define CELLFORMAT_H

#include "sheet.h"

/* OPIE */
#include <qpe/fontdatabase.h>

/* QT */
#include <qdialog.h>
#include <qtabwidget.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qpushbutton.h>

class BorderEditor: public QFrame
{
    Q_OBJECT

    // QT objects
    QPen penTop, penBottom, penLeft, penRight, penHorz, penVert;

    // Private functions
    void drawContents(QPainter *p);

    // Reimplemented QFrame functions
    void mouseReleaseEvent(QMouseEvent *e);

public:
    // Definitions
    enum BorderArea {None, Top, Bottom, Left, Right, Horz, Vert};

    BorderEditor(QWidget *parent=0);
    ~BorderEditor();

    void setPen(const QPen &pen, BorderArea area);
    QPen getPen(BorderArea area);
    QPen getDefaultPen()                      { return QPen(Qt::black, 1, Qt::DotLine); }

signals:
    void clicked(BorderEditor::BorderArea);
};

class CellFormat: public QDialog
{
    Q_OBJECT

    enum comboType {COMBO_OTHER, COMBO_WIDTH, COMBO_FONT, COMBO_SIZE, COMBO_STYLE, COMBO_COLOR, COMBO_VALIGN, COMBO_HALIGN};

    // QT objects
    QBoxLayout *box;
    QTabWidget *tabs;
    QWidget *widgetBorders, *widgetBackground, *widgetFont, *widgetAlignment;
    QComboBox *comboBordersWidth, *comboBordersColor, *comboBackgroundColor, *comboBackgroundStyle, *comboFontColor, *comboFontSize, *comboFontFamily, *comboAlignmentVertical, *comboAlignmentHorizontal;
    QCheckBox *checkFontBold, *checkFontItalic, *checkAlignmentWrap;
    QPushButton *buttonBordersDefaults, *buttonBackgroundDefaults, *buttonFontDefaults, *buttonAlignmentDefaults;
    QFrame *frameBackground, *frameFont, *frameAlignment;
    QBrush brushBackground;
    QFont fontFont;
    QColor fontColor;
    Qt::AlignmentFlags textAlignment;
    FontDatabase fontDB;

    // Other objects & variables
    Sheet *sheet;
    BorderEditor *borderEditor;
    bool changedFont, changedAlign, changedBrush;

    // Private functions
    void createWidthCombo(QComboBox *combo);
    void createFontCombo(QComboBox *combo);
    void createHAlignCombo(QComboBox *combo);
    void createVAlignCombo(QComboBox *combo);
    void createStyleCombo(QComboBox *combo);
    void createSizeCombo(QComboBox *combo);
    void createColorCombo(QComboBox *combo);
    QComboBox *createCombo(comboType type, QWidget *parent, const QString &caption, int y);

    int findHAlignIndex(Qt::AlignmentFlags flag);
    int findVAlignIndex(Qt::AlignmentFlags flag);
    int findComboItemIndex(QComboBox *combo, const QString &item);
    int findColorIndex(const QColor &color);
    int findBrushStyleIndex(Qt::BrushStyle style);

private slots:
    void borderClicked(BorderEditor::BorderArea area);
    void backgroundClicked(int index);
    void fontClicked(bool on);
    void fontClicked(int index);
    void alignClicked(bool on);
    void alignClicked(int index);
    void slotBordersDefaults();
    void slotBackgroundDefaults();
    void slotFontDefaults();
    void slotAlignmentDefaults();

public:
    CellFormat(QWidget *parent=0);
    ~CellFormat();

    int exec(Sheet *s);

    void setTextAlign(Qt::AlignmentFlags flags);
    void setTextFont(const QFont &font, const QColor &color);
    void setBrushBackground(const QBrush &brush);
};

#endif
