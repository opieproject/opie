/*
                             This file is part of the Opie Project
                             Copyright (C) Maximillian Reiﬂ <harlekin@handhelds.org>
              =.             Copyright (C) The Opie Team <opie-devel@handhelds.org>
            .=l.
† † † † † †.>+-=
†_;:, † † .> † †:=|.         This program is free software; you can
.> <`_, † > †. † <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.-- † :           the terms of the GNU Library General Public
.="- .-=="i, † † .._         License as published by the Free Software
†- . † .-<_> † † .<>         Foundation; either version 2 of the License,
† † †._= =} † † † :          or (at your option) any later version.
† † .%`+i> † † † _;_.
† † .i_,=:_. † † †-<s.       This program is distributed in the hope that
† † †+ †. †-:. † † † =       it will be useful,  but WITHOUT ANY WARRANTY;
† † : .. † †.:, † † . . .    without even the implied warranty of
† † =_ † † † †+ † † =;=|`    MERCHANTABILITY or FITNESS FOR A
† _.=:. † † † : † †:=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.= † † † = † † † ;      Library General Public License for more
++= † -. † † .` † † .:       details.
†: † † = †...= . :.=-
†-. † .:....=;==+<;          You should have received a copy of the GNU
† -_. . . † )=. †=           Library General Public License along with
† † -- † † † †:-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include <opie2/oclickablelabel.h>

using namespace Opie::Ui;

/**
 * This constructs the clickable ButtonLabel
 *
 * @param parent The parent of this label
 * @param name A name of this label @see QObject
 * @param fl The windowing flags
 */
OClickableLabel::OClickableLabel(QWidget* parent, const char* name, WFlags fl)
                :QLabel(parent,name,fl)
{
    textInverted=false;
    isToggle=false;
    isDown=false;
    showState(false);
    setFrameShadow(Sunken);
}

/**
 * This method makes the label behave as a toggle button
 *
 * @param t Whether or not to behave like a toggle button
 */
void OClickableLabel::setToggleButton(bool t)
{
    isToggle=t;
}

/**
 * @internal
 */
void OClickableLabel::mousePressEvent( QMouseEvent * /*e*/ )
{
    if (isToggle && isDown)
    {
        showState(false);
    }
    else
    {
        showState(true);
    }
}

/**
 * @internal
 */
void OClickableLabel::mouseReleaseEvent( QMouseEvent *e )
{
    if (rect().contains(e->pos()) && isToggle) isDown=!isDown;

    if (isToggle && isDown)
    {
        showState(true);
    }
    else
    {
        showState(false);
    }

    if (rect().contains(e->pos()))
    {
        if (isToggle)
        {
            emit toggled(isDown);
        }
        emit clicked();
    }
}

/**
 * @internal
 */
void OClickableLabel::mouseMoveEvent( QMouseEvent *e )
{
    if (rect().contains(e->pos()))
    {
        if (isToggle && isDown)
        {
            showState(false);
        }
        else
        {
            showState(true);
        }
    }
    else
    {
        if (isToggle && isDown)
        {
            showState(true);
        }
        else
        {
            showState(false);
        }
    }
}

/**
 * this toggles the label and inverts the color of
 * the label
 * @param on
 */
void OClickableLabel::showState(bool on)
{
    if (on)
    {
        //setFrameShape(Panel);
        setInverted(true);
        setBackgroundMode(PaletteHighlight);
    }
    else
    {
        //setFrameShape(NoFrame);
        setInverted(false);
        setBackgroundMode(PaletteBackground);
    }
    repaint();
}

void OClickableLabel::setInverted(bool on)
{
    if ( (!textInverted && on) || (textInverted && !on) )
    {
        QPalette pal=palette();
        QColor col=pal.color(QPalette::Normal, QColorGroup::Foreground);
        col.setRgb(255-col.red(),255-col.green(),255-col.blue());
        pal.setColor(QPalette::Normal, QColorGroup::Foreground, col);
        setPalette(pal);
        textInverted=!textInverted;
    }
}

/**
 * @param on if the Label is down or up
 */
void OClickableLabel::setOn(bool on)
{
    isDown=on;
    showState(isDown);
}
