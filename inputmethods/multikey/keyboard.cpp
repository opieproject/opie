/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
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
**********************************************************************/

#include "keyboard.h"
#include "configdlg.h"

#include <qpe/global.h>
#include <qpe/qcopenvelope_qws.h>

#include <qwindowsystem_qws.h>
#include <qpainter.h>
#include <qfontmetrics.h>
#include <qtimer.h>
#include <qpe/qpeapplication.h>
#include <qpe/config.h>
#include <ctype.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qstringlist.h>
#include <iostream.h>

#include <sys/utsname.h>


/* Keyboard::Keyboard {{{1 */
Keyboard::Keyboard(QWidget* parent, const char* _name, WFlags f) :
    QFrame(parent, _name, f),  shift(0), lock(0), ctrl(0),
    alt(0), useLargeKeys(TRUE), usePicks(0), useRepeat(0), pressedKeyRow(-1), pressedKeyCol(-1),
    unicode(-1), qkeycode(0), modifiers(0), schar(0), mchar(0), echar(0),
    configdlg(0)

{

    // get the default font
    Config *config = new Config( "qpe" );
    config->setGroup( "Appearance" );
    QString familyStr = config->readEntry( "FontFamily", "fixed" );
    delete config;

    config = new Config("multikey");
    config->setGroup ("general");
    usePicks = config->readBoolEntry ("usePickboard", "0"); // default closed
    useRepeat = config->readBoolEntry ("useRepeat", "1");
    delete config;


    setFont( QFont( familyStr, 10 ) );

    picks = new KeyboardPicks( this );
    picks->setFont( QFont( familyStr, 10 ) );
    picks->initialise();
    if (usePicks) {

        QObject::connect( picks, SIGNAL(key(ushort,ushort,ushort,bool,bool) ),
            this, SIGNAL(key(ushort,ushort,ushort,bool,bool)) );

    } else picks->hide();

    loadKeyboardColors();

    keys = new Keys();

    repeatTimer = new QTimer( this );
    connect( repeatTimer, SIGNAL(timeout()), this, SLOT(repeat()) );

}

Keyboard::~Keyboard() {

    if ( configdlg ) { 
        delete (ConfigDlg *) configdlg; 
        configdlg = 0;
    }

}

/* Keyboard::resizeEvent {{{1 */
void Keyboard::resizeEvent(QResizeEvent*)
{
    int ph = picks->sizeHint().height();
    picks->setGeometry( 0, 0, width(), ph );
    keyHeight = (height()-(usePicks ? ph : 0))/5;

    int nk; // number of keys?
    if ( useLargeKeys ) {
	nk = 15;
    } else {
	nk = 19;
    }
    defaultKeyWidth = (width()/nk)/2;
    xoffs = (width()-defaultKeyWidth*nk)/2; // empty key spaces?

}

/* KeyboardPicks::initialize {{{1 */
void KeyboardPicks::initialise()
{
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed));
    mode = 0;
    dc = new KeyboardConfig(this);
    configs.append(dc);
}

/* KeyboardPicks::sizeHint {{{1 */
QSize KeyboardPicks::sizeHint() const
{
    return QSize(240,fontMetrics().lineSpacing());
}


/* KeyboardConfig::generateText {{{1 */
void  KeyboardConfig::generateText(const QString &s)
{
#if defined(Q_WS_QWS) || defined(_WS_QWS_)
    for (int i=0; i<(int)backspaces; i++) {
	parent->emitKey( 0, Qt::Key_Backspace, 0, true, false );
	parent->emitKey( 0, Qt::Key_Backspace, 0, false, false );
    }
    for (int i=0; i<(int)s.length(); i++) {
	parent->emitKey( s[i].unicode(), 0, 0, true, false );
	parent->emitKey( s[i].unicode(), 0, 0, false, false );
    }
    parent->emitKey( 0, Qt::Key_Space, 0, true, false );
    parent->emitKey( 0, Qt::Key_Space, 0, false, false );
    backspaces = 0;
#endif
}




/* Keyboard::paintEvent {{{1 */
void Keyboard::paintEvent(QPaintEvent* e)
{
    QPainter painter(this);
    painter.setClipRect(e->rect());
    drawKeyboard( painter );
    picks->dc->draw( &painter );
}


/* Keyboard::drawKeyboard {{{1 */

void Keyboard::drawKeyboard(QPainter &p, int row, int col)
{


    if (row != -1 && col != -1) { //just redraw one key

        int x = 0;
        for (int i = 0; i < col; i++) {

            x += keys->width(row, i) * defaultKeyWidth;
        }
        int y = (row - 1) * keyHeight + (usePicks ? picks->height() : 0);

        int keyWidth = keys->width(row, col);

        p.fillRect(x + 1, y + 1, 
                   keyWidth * defaultKeyWidth - 1, keyHeight - 1, 
                   pressed || keys->pressed(row, col) ? keycolor_pressed : keycolor);

        QPixmap *pix = keys->pix(row,col);

        ushort c = keys->uni(row, col);

        p.setPen(textcolor);
        if (!pix)
            p.drawText(x, y, 
               defaultKeyWidth * keyWidth, keyHeight,
               AlignCenter, ((shift || lock) && keys->shift(c)) ? (QChar)keys->shift(c) : (QChar)c);
        else
            // center the image in the middle of the key
            p.drawPixmap( x + (defaultKeyWidth * keyWidth - pix->width())/2, 
                          y + (keyHeight - pix->height())/2 + 1, 
                          *pix );

        // this fixes the problem that the very right end of the board's vertical line
        // gets painted over, because it's one pixel shorter than all other keys
        p.setPen(keycolor_lines);
        p.drawLine(width() - 1, 0, width() - 1, height());

    } else {


    p.fillRect(0, 0, width(), height(), keycolor);

    for (row = 1; row <= 5; row++) {

        int x = 0;
        int y = (row - 1) * keyHeight + (usePicks ? picks->height() : 0);

        p.setPen(keycolor_lines);
        p.drawLine(x, y, x + width(), y);

        for (int col = 0; col < keys->numKeys(row); col++) {

            QPixmap *pix = keys->pix(row, col);
            int keyWidth = keys->width(row, col);


            int keyWidthPix = defaultKeyWidth * keyWidth;

            if (keys->pressed(row, col)) 
                p.fillRect(x+1, y+1, keyWidthPix - 1, 
                           keyHeight - 1, keycolor_pressed);

            ushort c = keys->uni(row, col);

            if (!pix) {
                p.setPen(textcolor);
                p.drawText(x, y, 
                   keyWidthPix, keyHeight,
                   AlignCenter, ((shift || lock) && keys->shift(c)) ? (QChar)keys->shift(c) : (QChar)c);
            }
            else {
                // center the image in the middle of the key
                p.drawPixmap( x + (keyWidthPix - pix->width())/2, 
                              y + (keyHeight - pix->height())/2 + 1, 
                              QPixmap(*pix) );
            }

            p.setPen(keycolor_lines);
            p.drawLine(x, y, x, y + keyHeight);

            x += keyWidthPix;
        }


    }
    p.drawLine(0, height() - 1, width(), height() - 1);
    p.drawLine(width() - 1, 0, width() - 1, height());
    }

}


/* Keyboard::mousePressEvent {{{1 */
void Keyboard::mousePressEvent(QMouseEvent *e)
{
    int row = (e->y() - (usePicks ? picks->height() : 0)) / keyHeight + 1;
    if (row > 5) row = 5;

    // figure out the column
    int col = 0; 
    for (int w = 0; e->x() >= w; col++)
        if (col < keys->numKeys(row)) // it segfaults if it trys to read past numKeys
            w += keys->width(row,col) * defaultKeyWidth;
        else break;

    if (col <= 0) return;

    col --; // rewind one...

    qkeycode = keys->qcode(row, col);
    unicode = keys->uni(row, col);

    // might need to repaint if two or more of the same keys.
    // should be faster if just paint one key even though multiple keys exist.
    bool need_repaint = FALSE; 

    if (unicode == 0) { // either Qt char, or nothing

        if (qkeycode == Qt::Key_F1) { // toggle the pickboard

            if ( configdlg ) { 
                delete (ConfigDlg *) configdlg; 
                configdlg = 0;
            }
            else {
               configdlg = new ConfigDlg ();
               connect(configdlg, SIGNAL(setMapToDefault()), 
                       this, SLOT(setMapToDefault()));
               connect(configdlg, SIGNAL(setMapToFile(QString)), 
                       this, SLOT(setMapToFile(QString)));
               connect(configdlg, SIGNAL(pickboardToggled(bool)), 
                       this, SLOT(togglePickboard(bool)));
               connect(configdlg, SIGNAL(repeatToggled(bool)),
                       this, SLOT(toggleRepeat(bool)));
               connect(configdlg, SIGNAL(reloadKeyboard()),
                       this, SLOT(reloadKeyboard()));
               configdlg->showMaximized();
               configdlg->show();
               configdlg->raise();
            }

        } else if (qkeycode == Qt::Key_Control) {

            if (ctrl) {

                *ctrl = 0;
                ctrl = 0;

            } else {

                ctrl = keys->pressedPtr(row, col);
                need_repaint = TRUE;
                *ctrl = !keys->pressed(row, col);

            }

        } else if (qkeycode == Qt::Key_Alt) {

            if (alt) {
                *alt = 0;
                alt = 0;

            } else {

                alt = keys->pressedPtr(row, col);
                need_repaint = TRUE;
                *alt = !keys->pressed(row, col);
            }

        } else if (qkeycode == Qt::Key_Shift) {
            need_repaint = TRUE;

            if (shift) {
                *shift = 0;
                shift = 0;
            }
            else {
                shift = keys->pressedPtr(row, col);
                *shift = 1;
                if (lock) {
                    *lock = 0;
                    lock = 0;
                }
            }

        } else if (qkeycode == Qt::Key_CapsLock) {
            need_repaint = TRUE;

            if (lock) {
                *lock = 0;
                lock = 0;
            }
            else {
                lock = keys->pressedPtr(row, col);;
                *lock = 1;
                if (shift) {
                    *shift = 0;
                    shift = 0;
                }
            }

        }

    }
    else { // normal char
        if ((shift || lock) && keys->shift(unicode)) {
            unicode = keys->shift(unicode);
        }
    }

    // korean parsing
    if (keys->lang == "ko") {

        unicode = parseKoreanInput(unicode);
    }

    modifiers = (ctrl ? Qt::ControlButton : 0) | (alt ? Qt::AltButton : 0);

    if ('A' <= unicode && unicode <= 'z' && modifiers) {

        qkeycode = QChar(unicode).upper();
        unicode = qkeycode - '@';
    }

    QWSServer::sendKeyEvent(unicode, qkeycode, modifiers, true, false); 

    // pickboard stuff
    if (usePicks) {

        KeyboardConfig *dc = picks->dc;
    
        if (dc) {
            if (qkeycode == Qt::Key_Backspace) {
                dc->input.remove(dc->input.last()); // remove last input
                dc->decBackspaces();
            } else if ( qkeycode == Qt::Key_Return || QChar(unicode).isPunct() || QChar(unicode).isSpace() || unicode == 0) {
                dc->input.clear();
                dc->resetBackspaces();
            } else {
                dc->add(QString(QChar(unicode)));
                dc->incBackspaces();
            }
        }
        picks->repaint();
    }


    // painting
    pressed = TRUE;

    pressedKeyRow = row;
    pressedKeyCol = col;

    if (need_repaint) repaint(FALSE);
    else { // just paint the one key pressed



        QPainter p(this);
        drawKeyboard(p, row, col);

    }

    if (useRepeat) repeatTimer->start( 800 );
    //pressTid = startTimer(80);

}


/* Keyboard::mouseReleaseEvent {{{1 */
void Keyboard::mouseReleaseEvent(QMouseEvent*)
{
    pressed = FALSE;
    //if ( pressTid == 0 )
#if defined(Q_WS_QWS) || defined(_WS_QWS_)
    if ( unicode != -1 ) {
	emit key( unicode, qkeycode, modifiers, false, false );
	repeatTimer->stop();
    }
#endif
    if (shift && unicode != 0) {


        *shift = 0; // unpress shift key
        shift = 0;  // reset the shift pointer
        repaint(FALSE);

    }
    else 

    clearHighlight();
}

/* Keyboard::timerEvent {{{1 */

/* dont know what this does, but i think it is here so that if your screen
 * sticks (like on an ipaq) then it will stop repeating if you click another
 * key... but who knows what anything does in this thing anyway?

 void Keyboard::timerEvent(QTimerEvent* e)
{
    if ( e->timerId() == pressTid ) {
	killTimer(pressTid);
	pressTid = 0;
	if ( !pressed )
        cout << "calling clearHighlight from timerEvent\n";
	    //clearHighlight();
    }
}
*/

void Keyboard::repeat()
{

  repeatTimer->start( 200 );
  emit key( unicode, qkeycode, modifiers, true, true );
}

void Keyboard::clearHighlight()
{
    if ( pressedKeyRow >= 0 && pressedKeyCol >= 0) {
	    int tmpRow = pressedKeyRow;
        int tmpCol = pressedKeyCol;

	    pressedKeyRow = -1;
	    pressedKeyCol = -1;

	    QPainter p(this);
	    drawKeyboard(p, tmpRow, tmpCol);
    }
}


/* Keyboard::sizeHint {{{1 */
QSize Keyboard::sizeHint() const
{
    QFontMetrics fm=fontMetrics();
    int keyHeight = fm.lineSpacing() + 2;

    return QSize( 240, keyHeight * 5 + (usePicks ? picks->sizeHint().height() : 0) + 1);
}


void Keyboard::resetState()
{
    schar = mchar = echar = 0;
    picks->resetState();
}

/* Keyboard::togglePickboard {{{1 */
void Keyboard::togglePickboard(bool on_off)
{
    usePicks = on_off;
    if (usePicks) {
        picks->show();
        //move(x(), y() - picks->height()); // not required anymore because QCopChannel::send
        //adjustSize();
        QObject::connect( picks, SIGNAL(key(ushort,ushort,ushort,bool,bool) ),
            this, SIGNAL(key(ushort,ushort,ushort,bool,bool)) );
    } else {

        picks->hide();
        picks->resetState();
        //move(x(), y() + picks->height());
        //adjustSize();
        QObject::disconnect( picks, SIGNAL(key(ushort,ushort,ushort,bool,bool) ),
            this, SIGNAL(key(ushort,ushort,ushort,bool,bool)) );

    }
    /*
     * this closes && opens the input method
     */
    QCopChannel::send ("QPE/TaskBar", "hideInputMethod()");
    QCopChannel::send ("QPE/TaskBar", "showInputMethod()");
}

void Keyboard::toggleRepeat(bool on) {

    useRepeat = on;
    cout << "setting useRepeat to: " << useRepeat << "\n";
}

/* Keyboard::setMapTo ... {{{1 */
void Keyboard::setMapToDefault() {


    /* load current locale language map */
    Config *config = new Config("locale");
    config->setGroup( "Language" );
    QString l = config->readEntry( "Language" , "en" );
    delete config;

    QString key_map = QPEApplication::qpeDir() + "/share/multikey/" 
            + l + ".keymap";

    /* save change to multikey config file */
    config = new Config("multikey");
    config->setGroup ("keymaps");
    config->writeEntry ("current", key_map); // default closed
    delete config;

    delete keys;
    keys = new Keys(key_map);

    // have to repaint the keyboard
    repaint(FALSE);
}

void Keyboard::setMapToFile(QString map) {

    /* save change to multikey config file */
    Config *config = new Config("multikey");
    config->setGroup ("keymaps");
    config->writeEntry ("current", map); // default closed

    delete config;

    delete keys;
    if (QFile(map).exists()) 
        keys = new Keys(map);
    else 
        keys = new Keys();

    repaint(FALSE);

}

/* Keybaord::reloadKeyboard {{{1 */
void Keyboard::reloadKeyboard() {

    // reload colors and redraw
    loadKeyboardColors();
    repaint();

}

void Keyboard::loadKeyboardColors() {

    Config config ("multikey");
    config.setGroup("colors");

    QStringList color;
    color = config.readListEntry("keycolor", QChar(','));
    if (color.isEmpty()) {
        color = QStringList::split(",", "240,240,240");
        config.writeEntry("keycolor", color.join(","));

    }
    keycolor = QColor(color[0].toInt(), color[1].toInt(), color[2].toInt());

    color = config.readListEntry("keycolor_pressed", QChar(','));
    if (color.isEmpty()) {
        color = QStringList::split(",", "171,183,198");
        config.writeEntry("keycolor_pressed", color.join(","));

    }
    keycolor_pressed = QColor(color[0].toInt(), color[1].toInt(), color[2].toInt());

    color = config.readListEntry("keycolor_lines", QChar(','));
    if (color.isEmpty()) {
        color = QStringList::split(",", "138,148,160");
        config.writeEntry("keycolor_lines", color.join(","));

    }
    keycolor_lines = QColor(color[0].toInt(), color[1].toInt(), color[2].toInt());

    color = config.readListEntry("textcolor", QChar(','));
    if (color.isEmpty()) {
        color = QStringList::split(",", "43,54,68");
        config.writeEntry("textcolor", color.join(","));

    }
    textcolor = QColor(color[0].toInt(), color[1].toInt(), color[2].toInt());

}

/* korean input functions {{{1 
 *
 * TODO
 * one major problem with this implementation is that you can't move the
 * cursor after inputing korean chars, otherwise it will eat up and replace
 * the char before the cursor you move to. fix that
 *
 * make backspace delete one single char, not the whole thing if still 
 * editing.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *
 * how korean input works
 *
 * all following chars means unicode char value and are in hex
 *
 * 초음 = schar (start char)
 * 중음 = mchar (middle char)
 * 끝음 = echar (end char)
 *
 * there are 19 schars. unicode position is at 1100 - 1112
 * there are 21 mchars. unicode position is at 1161 - 1175
 * there are 27 echars. unicode position is at 11a8 - 11c2
 *
 * the map with everything combined is at ac00 - d7a3
 *
 */

ushort Keyboard::parseKoreanInput (ushort c) {

	if ((c != 0 && (c < 0x1100 || 0x11c2 < c) && (c < 0xac00 || 0xd7a3 < c)) 
          ||
        (c == 0 && qkeycode != Qt::Key_Shift && Qt::Key_CapsLock != qkeycode
         && qkeycode != Qt::Key_Control && qkeycode != Qt::Key_Alt)) {

		schar = 0, mchar = 0, echar = 0;
        return c;
	}

	if ( 0x1100 <= c && c <= 0x1112 ) { // schar or echar was input

		if (schar == 0 || (schar != 0 && mchar == 0)) {
			schar = c; mchar = 0; echar = 0;
			return c;
		}
		else if (mchar != 0) {

			if (echar == 0) {

				if (!(echar = constoe(c))) {

					schar = c; mchar = 0; echar = 0; 
					return c;
				}

			}
			else { // must figure out what the echar is

				if (echar == 0x11a8) { // ㄱ

					if (c == 0x1100) 		echar = 0x11a9; // ㄱ + ㄱ
					else if (c == 0x1109) 	echar = 0x11aa; // ㄱ + ㅅ
					else { 
						schar = c; mchar = 0; echar = 0; 
						return c;
					}

				} else if (echar == 0x11ab) { // ㄴ

					if (c == 0x110c)  		echar = 0x11ac; // ㄴ + ㅈ
					else if (c == 0x1112)  	echar = 0x11ad; // ㄴ + ㅎ
					else {
						schar = c; mchar = 0; echar = 0; 
						return c;
					}

				} else if (echar == 0x11af) { // ㄹ

					if (c == 0x1100) 		echar = 0x11b0; // ㄹ + ㄱ
					else if (c == 0x1106) 	echar = 0x11b1; // ㄹ + ㅁ
					else if (c == 0x1107) 	echar = 0x11b2; // ㄹ + ㅂ
					else if (c == 0x1109) 	echar = 0x11b3; // ㄹ + ㅅ
					else if (c == 0x1110) 	echar = 0x11b4; // ㄹ + ㅌ
					else if (c == 0x1111) 	echar = 0x11b5; // ㄹ + ㅍ
					else if (c == 0x1112) 	echar = 0x11b6; // ㄹ + ㅎ
					else {
						schar = c; mchar = 0; echar = 0; 
						return c;
					}

				} else if (echar == 0x11b8) { // ㅂ

					if (c == 0x1109) 		echar = 0x11b9; // ㅂ + ㅅ
					else {
						schar = c; mchar = 0; echar = 0; 
						return c;
					}

				} else if (echar == 0x11ba) { // ㅅ 

					if (c == 0x1109) 		echar = 0x11bb; // ㅅ + ㅅ 
					else {
						schar = c; mchar = 0; echar = 0; 
						return c;
					}

				} else { // if any other char, cannot combine chars

					schar = c; mchar = 0; echar = 0;
					return c;
				}

				unicode = echar;
			}
		}

	}
	else if (0x1161 <= c && c <= 0x1175) { // mchar was input

		if (schar != 0 && mchar == 0) { mchar = c; }

		else if (schar != 0 && mchar != 0 && echar == 0) {

			switch (mchar) {
				case 0x1169:
					if (c == 0x1161) mchar = 0x116a;
					else if (c == 0x1162) mchar = 0x116b;
					else if (c == 0x1175) mchar = 0x116c;
					else {
						schar = 0; mchar = 0; echar = 0;
						return c;
					}
					break;
				case 0x116e:
					if (c == 0x1165) mchar = 0x116f;
					else if (c == 0x1166) mchar = 0x1170;
					else if (c == 0x1175) mchar = 0x1171;
					else {
						schar = 0; mchar = 0; echar = 0;
						return c;
					}
					break;
				case 0x1173:
					if (c == 0x1175) mchar = 0x1174;
					else {
						schar = 0; mchar = 0; echar = 0;
						return c;
					}
					break;
				default: 
					schar = 0; mchar = 0; echar = 0;
					return c;
			}
		}
		else if (schar != 0 && mchar != 0 && echar != 0) {

			emit key( 8, Qt::Key_Backspace, 0, true, false );

			ushort prev = 0;
			switch (echar) {
				/*
				case 0x11a9:
					prev = combineKoreanChars(schar, mchar, 0x11a8);
					schar = 0x1100; 
					break;
				*/
				case 0x11aa:
					prev = combineKoreanChars(schar, mchar, 0x11a8);
					schar = 0x1109;
					break;
				case 0x11ac:
					prev = combineKoreanChars(schar, mchar, 0x11ab);
					schar = 0x110c; 
					break;
				case 0x11ad:
					prev = combineKoreanChars(schar, mchar, 0x11ab);
					schar = 0x1112;
					break;
				case 0x11b0:
					prev = combineKoreanChars(schar, mchar, 0x11af);
					schar = 0x1100; 
					break;
				case 0x11b1:
					prev = combineKoreanChars(schar, mchar, 0x11af);
					schar = 0x1106;
					break;
				case 0x11b2:
					prev = combineKoreanChars(schar, mchar, 0x11af);
					schar = 0x1107; 
					break;
				case 0x11b3:
					prev = combineKoreanChars(schar, mchar, 0x11af);
					schar = 0x1109;
					break;
				case 0x11b4:
					prev = combineKoreanChars(schar, mchar, 0x11af);
					schar = 0x1110;
					break;
				case 0x11b9:
					prev = combineKoreanChars(schar, mchar, 0x11b8);
					schar = 0x1109;
					break;
				/*
				case 0x11bb:
					prev = combineKoreanChars(schar, mchar, 0x11ba);
					schar = 0x1109; 
					break;
				*/
				default: 

					if (constoe(echar)) {

						prev = combineKoreanChars(schar, mchar, 0);
						schar = constoe(echar); 
					} 
					break;
			}
			
			emit key( prev, prev, 0, true, false );

			mchar = c; echar = 0;

			return combineKoreanChars(schar, mchar, 0);
		
		} 
		else {
			schar = 0; mchar = 0; echar = 0;
			return c;
		}

	} 
	else /*if (c == ' ')*/ return c;


	// and now... finally delete previous char, and return new char
	emit key( 8, Qt::Key_Backspace, 0, true, false );


	return combineKoreanChars( schar, mchar, echar);

}

ushort Keyboard::combineKoreanChars(const ushort s, const ushort m, const ushort e) {

	return ((s - 0x1100) * 588) + ((m - 0x1161) * 28) + (e ? e - 0x11a7 : 0) + 0xac00;

}

ushort Keyboard::constoe(const ushort c) {

	// converts schars to echars if possible

	if (0x1100 <= c && c <= 0x1112) { // schar to echar

		switch (c) {
			case 0x1100: return 0x11a8;
			case 0x1101: return 0x11a9;
			case 0x1102: return 0x11ab;
			case 0x1103: return 0x11ae;
			case 0x1105: return 0x11af;
			case 0x1106: return 0x11b7;
			case 0x1107: return 0x11b8;
			case 0x1109: return 0x11ba;
			case 0x110a: return 0x11bb;
			case 0x110b: return 0x11bc;
			case 0x110c: return 0x11bd;
			case 0x110e: return 0x11be;
			case 0x110f: return 0x11bf;
			case 0x1110: return 0x11c0;
			case 0x1111: return 0x11c1;
			case 0x1112: return 0x11c2;
			default: return 0;

		}

	} else { //echar to schar

		switch (c) {
			case 0x11a8: return 0x1100;
			case 0x11a9: return 0x1101;
			case 0x11ab: return 0x1102;
			case 0x11ae: return 0x1103;
			case 0x11af: return 0x1105;
			case 0x11b7: return 0x1106;
			case 0x11b8: return 0x1107;
			case 0x11ba: return 0x1109;
			case 0x11bb: return 0x110a;
			case 0x11bc: return 0x110b;
			case 0x11bd: return 0x110c;
			case 0x11be: return 0x110e;
			case 0x11bf: return 0x110f;
			case 0x11c0: return 0x1110;
			case 0x11c1: return 0x1111;
			case 0x11c2: return 0x1112;
			default: return 0;

		}

	}
}


// Keys::Keys {{{1

Keys::Keys() {

    Config *config = new Config ("multikey");
    config->setGroup( "keymaps" );
    QString map = config->readEntry( "current" );
    delete config;

    if (map.isNull() || !(QFile(map).exists())) {

        Config *config = new Config("locale");
        config->setGroup( "Language" );
        QString l = config->readEntry( "Language" , "en" );
        delete config;
    
        map = QPEApplication::qpeDir() + "/share/multikey/" 
                + l + ".keymap";

    } 

    setKeysFromFile(map);
}

Keys::Keys(const char * filename) {

    setKeysFromFile(filename);
}

// Keys::setKeysFromFile {{{2
void Keys::setKeysFromFile(const char * filename) {

    QFile f(filename);

    if (f.open(IO_ReadOnly)) {

        QTextStream t(&f);
        int row;
        int qcode;
        ushort unicode;
        int width;
        QString buf;
        QString comment;
        char * xpm[256]; //couldnt be larger than that... could it?
        QPixmap *xpm2pix = 0;

        buf = t.readLine();
        while (buf) {

            // key definition
            if (buf.contains(QRegExp("^\\d+\\s+[0-1a-fx]+", FALSE, FALSE))) { 
            // no $1 type referencing!!! this implementation of regexp sucks

                // dont know of any sscanf() type funcs in Qt lib
                QTextStream tmp (buf, IO_ReadOnly);
                tmp >> row >> qcode >> unicode >> width >> comment;

                buf = t.readLine();
                int xpmLineCount = 0;
                xpm2pix = 0;

                // erase blank space
                while (buf.contains(QRegExp("^\\s*$")) && buf) buf = t.readLine();

                while (buf.contains(QRegExp("^\\s*\".*\""))) {

                    QString xpmBuf = buf.stripWhiteSpace();

                    xpm[xpmLineCount] = new char [xpmBuf.length()];

                    int j = 0;
                    for (ushort i = 0; i < xpmBuf.length(); i++) {
                        if (xpmBuf[i].latin1() != '"') {

                            ((char *)xpm[xpmLineCount])[j] = xpmBuf.at(i).latin1();
                            j++;
                        }

                    }
                    // have to close that facker up
                    ((char *)xpm[xpmLineCount])[j] = '\0';

                    xpmLineCount++;
                    buf = t.readLine();
                }
                if (xpmLineCount) {

                    xpm2pix = new QPixmap((const char **)xpm);
                    for (int i = 0; i < xpmLineCount; i++) 

                        delete [] (xpm[i]);

                }
                setKey(row, qcode, unicode, width, xpm2pix);
            }

            // shift map
            else if (buf.contains(QRegExp("^[0-9a-fx]+\\s+[0-9a-fx]+\\s*$", FALSE, FALSE))) {

                QTextStream tmp (buf, IO_ReadOnly);
                ushort lower, shift;
                tmp >> lower >> shift;

                shiftMap.insert(lower, shift);

                buf = t.readLine();
            }

            // other variables like lang & title
            else if (buf.contains(QRegExp("^\\s*[a-zA-Z]+\\s*=\\s*[a-zA-Z0-9/]+\\s*$", FALSE, FALSE))) {

                QTextStream tmp (buf, IO_ReadOnly);
                QString name, equals, value;

                tmp >> name >> equals >> value;

                if (name == "lang") {

                    lang = value;

                } 

                buf = t.readLine();
            }
            // comments
            else if (buf.contains(QRegExp("^\\s*#"))) {

                buf = t.readLine();

            } else { // blank line, or garbage

                buf = t.readLine();

            }

        }
        f.close();
    }

}

// Keys::setKey {{{2
void Keys::setKey(const int row, const int qcode, const ushort unicode, 
                    const int width, QPixmap *pix) {

    Key * key;
    key = new Key; 
    key->qcode = qcode;
    key->unicode = unicode;
    key->width = width;

    // share key->pressed between same keys
    bool found = 0;
    for (int i = 1; i <= 5; i++) {
        for (unsigned int j = 0; j < keys[i].count(); j++)
            if (keys[i].at(j)->qcode == qcode && keys[i].at(j)->unicode == unicode) {
               
                key->pressed = keys[i].at(j)->pressed;
                found = 1;
            }

    }
    if (!found) {

        key->pressed = new bool;
        *(key->pressed) = 0;
    }

    key->pix = pix;


    keys[row].append(key);
}

// Keys::~Keys {{{2
Keys::~Keys() {

    for (int i = 1; i <= 5; i++) 
        for (unsigned int j = 0; j < keys[i].count(); j++)
            delete keys[i].at(j);

}

// Keys:: other functions {{{2
int Keys::width(const int row, const int col) {
    
    return keys[row].at(col)->width;

}
ushort Keys::uni(const int row, const int col) {

    return keys[row].at(col)->unicode;

}

int Keys::qcode(const int row, const int col) {

    return keys[row].at(col)->qcode;
}

QPixmap *Keys::pix(const int row, const int col) {

    return keys[row].at(col)->pix;

}
bool Keys::pressed(const int row, const int col) {

    return *(keys[row].at(col)->pressed);
}

int Keys::numKeys(const int row) {

    return keys[row].count();
}

void Keys::setPressed(const int row, const int col, const bool pressed) {

    *(keys[row].at(col)->pressed) = pressed;
}

ushort Keys::shift(const ushort uni) {

    if (shiftMap[uni]) {

        return shiftMap[uni];
    }
    else 
        return 0;

}

bool *Keys::pressedPtr(const int row, const int col) {

    return keys[row].at(col)->pressed;
}
