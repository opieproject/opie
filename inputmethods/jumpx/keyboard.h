/**************************************************************************************94x78**
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*********************************************************************************************/
#include <qframe.h>
#include <qpixmap.h>
#include <qtimer.h>

class Keyboard : public QFrame
{
    Q_OBJECT
public:
    Keyboard( QWidget* parent=0, const char* name=0, WFlags f=0 );

    void resetState();

    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void resizeEvent(QResizeEvent*);
    void paintEvent(QPaintEvent* e);
    //void timerEvent(QTimerEvent* e);

    QSize sizeHint() const;

signals:
    void key( ushort unicode, ushort qcode, ushort modifiers, bool, bool );

private slots:
    void delayTimerDone();
    void rateTimerDone();

private:
    int shift; // 0, 1, 2
    int paren; // 0, 1, 2
    int ctrl;  // 0, 1
    int alt;   // 0, 1

    int pressedKeyUnicode, pressedKeyQcode, pressedMod;
    int pressedx, pressedy, pressedw, pressedh;
    bool isnoncont;
    int pressed2x, pressed2y, pressed2w, pressed2h;

    int slideKeyUnicodeH, slideKeyQcodeH, slideKeyUnicodeV, slideKeyQcodeV;
    bool enableMouseTracking;
    QPixmap *slidePix, *slidePixH, *slidePixV;

    QPixmap releasedPlain;
    QPixmap releasedShift;
    QPixmap releasedParen;

    QPixmap pressedPlain;
    QPixmap pressedShift;
    QPixmap pressedParen;
    QPixmap pressedDigit;

    QPixmap offscreen;

    QPixmap *releasedPix;
    QPixmap *pressedPix;

    QTimer *delayTimer;
    QTimer *rateTimer;
};
