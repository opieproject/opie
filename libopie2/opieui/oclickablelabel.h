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

#ifndef OCLICKABLELABEL_H
#define OCLICKABLELABEL_H

/* QT */
#include <qlabel.h>

namespace Opie{
namespace Ui  {

/**
 * This class is a special QLabel which can behave
 * as a QPushButton or QToggleButton.
 * The reason to use a clickable is if you want to save space
 * or you want to skip the border of a normal button
 *
 * <pre>
 *  QLabel* lbl = new OClickableLabel( parent, "PushLabel" );
 *  lbl->setPixmap( "config" );
 *  QWhatsThis::add( lbl, tr("Click here to do something") );
 * </pre>
 *
 * @short A Label behaving as button
 * @author Hakan Ardo, Maximillian Reiﬂ ( harlekin@handhelds.org )
 * @see QLabel
 * @see QPushButton
 * @see QToggleButton
 * @version 1.0
 */

class OClickableLabel: public QLabel
{
    Q_OBJECT
public:
    OClickableLabel(QWidget* parent = 0, const char* name = 0,
                    WFlags fl = 0);
    void setToggleButton(bool t);

protected:
    /** @internal */
    void mousePressEvent( QMouseEvent *e );
    /** @internal */
    void mouseReleaseEvent( QMouseEvent *e );
    /** @internal */
    void mouseMoveEvent( QMouseEvent *e );

public slots:
    void setOn(bool on);
signals:
    /**
     * emitted when the labels gets clicked
     */
    void clicked();

    /**
     * emitted when the labels gets toggled
     * @param on the new new state of the label
     */
    void toggled(bool on);
private:
    bool isToggle : 1;
    bool isDown : 1;
    bool textInverted : 1;

    void showState(bool on);
    void setInverted(bool on);

private:
    class Private;
    Private *d; // private d pointer
};

}
}

#endif
