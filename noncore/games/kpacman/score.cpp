
#include "portable.h"

#if defined( KDE2_PORT )
#include <score.h>
#include <score.moc>

#include <kaccel.h>
#include <kapp.h>
#include <kconfig.h>
#include <kstddirs.h>
#include <kmessagebox.h>
#elif defined( QPE_PORT )
#include <qaccel.h>
#include "config.h"
#include "score.h"
#endif

#include <stdlib.h>
#include <ctype.h>

#include <qpixmap.h>
#include <qstring.h>
#include <qdstream.h>
#include <qkeycode.h>
#include <qtimer.h>
#include <qfileinfo.h>

#include "bitfont.h"

Score::Score(QWidget *parent, const char *name, int Scheme, int Mode, Bitfont *font) : QWidget(parent, name)
{
    setFocusPolicy(QWidget::StrongFocus);

    paused = FALSE;

    lastScore = -1;
    lastPlayer = -1;

    cursorBlinkTimer = 0;
    cursorBlinkMS = -1;
    cursor.x = -1;
    cursor.y = -1;
    cursor.on = FALSE;
    cursor.chr = QChar('?');

    initKeys();

    scheme = Scheme;
    mode = Mode;
    confScheme();

    bitfont = font;

    highscoreFile.setName(locateHighscoreFilePath().filePath());
    read();

    for (int p = 0; p < maxPlayer; p++) {
        playerScore[p] = 0;
        playerName[p] = getenv("LOGNAME");
        if (playerName[p].length() < minPlayerNameLength)
            playerName[p].setExpand(minPlayerNameLength-1, ' ');

        for (uint i = 0; i < playerName[p].length(); i++)
            if (playerName[p].at(i) < bitfont->firstChar() ||
                playerName[p].at(i) > bitfont->lastChar())
                playerName[p].at(i) = playerName[p].at(i).upper();
    }
}

Score::~Score()
{
    // write();
}

void Score::paintEvent( QPaintEvent *e)
{
    if (rect(1, 0, tr("  1UP ")).intersects(e->rect())) {
        QPixmap pix;
        QColor fg = BLACK;
        if (cursor.on || paused || lastPlayer != 0)
            fg = WHITE;
        pix = bitfont->text(tr("  1UP "), fg, BLACK);
        bitBlt(this, x(1), y(0), &pix);
    }

    if (rect(8, 0, tr(" HIGH SCORE ")).intersects(e->rect())) {
            QPixmap pix = bitfont->text(tr(" HIGH SCORE "), WHITE, BLACK);
        bitBlt(this, x(8), y(0), &pix);
    }

    if (maxPlayer > 1 && rect(21, 0, tr("  2UP ")).intersects(e->rect())) {
        QPixmap pix;
        QColor fg = BLACK;
        if (cursor.on || paused || lastPlayer != 1)
            fg = WHITE;
        pix = bitfont->text(tr("  2UP "), fg, BLACK);
        bitBlt(this, x(21), y(0), &pix);
    }

    QString s;

    s.sprintf("%6d0", playerScore[0]/10);
    if (rect(0, 1, s).intersects(e->rect())) {
            QPixmap pix = bitfont->text(s, WHITE, BLACK);
            bitBlt(this, x(0), y(1), &pix);
    }

    s.sprintf("%8d0", HighScore/10);
    if (rect(8, 1, s).intersects(e->rect())) {
            QPixmap pix = bitfont->text(s, WHITE, BLACK);
            bitBlt(this, x(8), y(1), &pix);
    }

    if (lastScore >= 0) {
        if (rect(1, 4*1.25, tr("     CONGRATULATIONS      ")).intersects(e->rect())) {
            QPixmap pix = bitfont->text(tr("     CONGRATULATIONS      "), YELLOW, BLACK);
            bitBlt(this, x(1), y(4*1.25), &pix);
        }
        if (rect(1, 6*1.25, tr("    YOU HAVE ARCHIEVED    ")).intersects(e->rect())) {
            QPixmap pix = bitfont->text(tr("    YOU HAVE ARCHIEVED    "), CYAN, BLACK);
            bitBlt(this, x(1), y(6*1.25), &pix);
        }
        if (rect(1, 7*1.25, tr("  A SCORE IN THE TOP 10.  ")).intersects(e->rect())) {
            QPixmap pix = bitfont->text(tr("  A SCORE IN THE TOP 10.  "), CYAN, BLACK);
            bitBlt(this, x(1), y(7*1.25), &pix);
        }
        if (rect(1, 8*1.25, tr("                          ")).intersects(e->rect())) {
            QPixmap pix = bitfont->text(tr("                          "), CYAN, BLACK);
            bitBlt(this, x(1), y(8*1.25), &pix);
        }
    }

    if (rect(1, 9.5*1.25, tr("RNK   SCORE  NAME   DATE")).intersects(e->rect())) {
            QPixmap pix = bitfont->text(tr("RNK   SCORE  NAME   DATE"), WHITE, BLACK);
        bitBlt(this, x(1), y(9.5*1.25), &pix);
    }

    for (int i = 0; i < 10; i++) {
        s.sprintf("%2d%9d  %-3.3s  %-8.8s",
            i+1, hallOfFame[i].points, hallOfFame[i].name.utf8().data(),
            formatDate(hallOfFame[i].moment.date()).data());
        if (rect(1, (11+i)*1.25, s).intersects(e->rect())) {
            QPixmap pix = bitfont->text(s, (i == lastScore) ? YELLOW : WHITE, BLACK);
            bitBlt(this, x(1), y((11+i)*1.25), &pix);
        }
    }

    if (cursor.x != -1 && cursor.y != -1 && cursor.on) {
        if (rect(cursor.x, (cursor.y*1.25), cursor.chr).intersects(e->rect())) {
            QPixmap pix = bitfont->text(cursor.chr, BLACK, YELLOW);
            bitBlt(this, x(cursor.x), y(cursor.y*1.25), &pix);
        }
    }

    if (paused) {

        QPixmap pix = bitfont->text(tr("PAUSED"), RED, BLACK);
        QRect r = bitfont->rect(tr("PAUSED"));
        r.moveCenter(QPoint(this->width()/2, this->height()/2));

        bitBlt(this, r.x(), r.y(), &pix);
    }
}

void Score::timerEvent(QTimerEvent*)
{
    cursor.on = !cursor.on;

    if (paused)
        return;

    if (cursor.x != -1 && cursor.y != -1)
        repaint(rect(cursor.x, cursor.y*1.25, cursor.chr), FALSE);
    scrollRepeat = FALSE;

    if (lastPlayer == 0)
        repaint(rect(1, 0, tr("  1UP ")), FALSE);

    if (lastPlayer == 1)
        repaint(rect(21, 0, tr("  2UP ")), FALSE);
}

void Score::keyPressEvent(QKeyEvent *k)
{
    if (lastScore < 0 || lastPlayer < 0 || lastPlayer >= maxPlayer || paused) {
        k->ignore();
        return;
    }

    int x = cursor.x;
    int y = cursor.y;

    uint key = k->key();

    if (scrollRepeat && (key == UpKey || key == Key_Up || key == DownKey || key == Key_Down)) {
        k->ignore();
        return;
    }

    if (key != Key_Return) {
        if (key == RightKey || key == Key_Right)
            if (++cursor.x > 16)
                cursor.x = 14;
        if (key == LeftKey || key == Key_Left)
            if (--cursor.x < 14)
                cursor.x = 16;
        if (key == UpKey || key == Key_Up)
            if (cursor.chr.unicode() < bitfont->lastChar())
                cursor.chr = cursor.chr.unicode()+1;
            else
                cursor.chr = bitfont->firstChar();
        if (key == DownKey || key == Key_Down)
            if (cursor.chr.unicode() > bitfont->firstChar())
                cursor.chr = cursor.chr.unicode()-1;
            else
                cursor.chr = bitfont->lastChar();

        if (cursor.x == x && cursor.y == y &&
            cursor.chr == hallOfFame[lastScore].name.at(cursor.x-14)) {
            uint ascii = k->ascii();

            if (ascii < bitfont->firstChar() || ascii > bitfont->lastChar())
                ascii = toupper(ascii);

            if (ascii >= bitfont->firstChar() && ascii <= bitfont->lastChar()) {
                cursor.chr = ascii;
                hallOfFame[lastScore].name.at(cursor.x-14) = cursor.chr;
                if (++cursor.x > 16)
                    cursor.x = 14;
            }
        }
    }

    if (key == Key_Return) {
        playerName[lastPlayer] = hallOfFame[lastScore].name;
        write();
        read();
        lastScore = -1;
        cursor.x = -1;
        cursor.y = -1;
//      killTimers();
        emit toggleNew();
        end();
    }

    if (x != cursor.x || y != cursor.y) {
        if (cursor.x != -1)
            cursor.chr = hallOfFame[lastScore].name.at(cursor.x-14);
        scrollRepeat = FALSE;
        repaint(rect(x, y*1.25, cursor.chr), FALSE);
    } else
        hallOfFame[lastScore].name.at(cursor.x-14) = cursor.chr;

    if (key == UpKey || key == Key_Up || key == DownKey || key == Key_Down)
       scrollRepeat = TRUE;
    else
       repaint(rect(cursor.x, cursor.y*1.25, cursor.chr), FALSE);
}

void Score::initKeys()
{
    APP_CONFIG_BEGIN( cfg );
    QString up("Up");
    up = cfg->readEntry("upKey", (const char*) up);
    UpKey    = KAccel::stringToKey(up);

    QString down("Down");
    down = cfg->readEntry("downKey", (const char*) down);
    DownKey  = KAccel::stringToKey(down);

    QString left("Left");
    left = cfg->readEntry("leftKey", (const char*) left);
    LeftKey  = KAccel::stringToKey(left);

    QString right("Right");
    right = cfg->readEntry("rightKey", (const char*) right);
    RightKey = KAccel::stringToKey(right);
    APP_CONFIG_END( cfg );
}

void Score::confTiming(bool defGroup)
{
    APP_CONFIG_BEGIN( cfg );
    if (defGroup || cfg->hasKey("CursorBlinkMS"))
        cursorBlinkMS = cfg->readNumEntry("CursorBlinkMS", 250);
    if (defGroup || cfg->hasKey("HallOfFameMS"))
        hallOfFameMS = cfg->readNumEntry("HallOfFameMS", 7000);
    if (defGroup || cfg->hasKey("AfterPauseMS"))
        afterPauseMS = cfg->readNumEntry("AfterPauseMS", 1000);
    APP_CONFIG_END( cfg );
}

void Score::confScheme()
{
    APP_CONFIG_BEGIN( cfg );
    SAVE_CONFIG_GROUP( cfg, oldgroup );
    QString newgroup;

    // if not set, read mode and scheme from the configfile
    if (mode == -1 && scheme == -1) {
        scheme = cfg->readNumEntry("Scheme", -1);
        mode = cfg->readNumEntry("Mode", -1);

        // if mode is not set in the defGroup-group, lookup the scheme group
        if (scheme != -1 || mode == -1) {
            newgroup.sprintf("Scheme %d", scheme);
            cfg->setGroup(newgroup);

            mode = cfg->readNumEntry("Mode", -1);
            RESTORE_CONFIG_GROUP( cfg, oldgroup );
        }
    }

    int oldCursorBlinkMS = cursorBlinkMS;

    confTiming();

    if (mode != -1) {
        newgroup.sprintf("Mode %d", mode);
        cfg->setGroup(newgroup);

        confTiming(FALSE);
    }

    if (scheme != -1) {
        newgroup.sprintf("Scheme %d", scheme);
        cfg->setGroup(newgroup);

        confTiming(FALSE);
    }

    if (cursorBlinkMS != oldCursorBlinkMS) {
        if (cursorBlinkTimer)
            killTimer(cursorBlinkTimer);
        cursorBlinkTimer = startTimer(cursorBlinkMS);
    }

    RESTORE_CONFIG_GROUP( cfg, oldgroup );
    APP_CONFIG_END( cfg );
}

void Score::setScheme(int Scheme, int Mode, Bitfont *font)
{
    mode = Mode;
    scheme = Scheme;

    confScheme();

    bitfont = font;

    for (int p = 0; p < maxPlayer; p++)
        for (uint i = 0; i < playerName[p].length(); i++)
            if (playerName[p].at(i) < bitfont->firstChar() ||
                playerName[p].at(i) > bitfont->lastChar())
                playerName[p].at(i) = playerName[p].at(i).upper();

    for (int i = 0; i < 10; i++)
        for (uint j = 0; j < hallOfFame[i].name.length(); j++)
            if (hallOfFame[i].name.at(j) < bitfont->firstChar() ||
                hallOfFame[i].name.at(j) > bitfont->lastChar())
                hallOfFame[i].name.at(j) = hallOfFame[i].name.at(j).upper();

    if (cursor.chr.unicode() < bitfont->firstChar() ||
        cursor.chr.unicode() > bitfont->lastChar())
        cursor.chr = cursor.chr.upper();
}

void Score::set(int score)
{
    set(score, 0);
}

void Score::set(int score, int player)
{
    if (player < 0 || player >= maxPlayer)
        return;

    lastPlayer = player;
    playerScore[lastPlayer] = score;

    QString s;

    s.sprintf("%6d0", playerScore[lastPlayer]/10);
    repaint(rect(0, 1, s), FALSE);

    if (score > HighScore) {
            HighScore = score;
            s.sprintf("%8d0", HighScore/10);
            repaint(rect(8, 1, s), FALSE);
    }
}

/*
 * Set the score for player after the game if over. If the score is in the
 * high scores then the hall of fame is updated (shifted) and the scoreboard
 * is shown.
 */

void Score::setScore(int level, int player)
{
    lastScore = -1;

    if (player < 0 || player >= maxPlayer || level == 0) {
        if (level != 0)
            emit toggleNew();
        QTimer::singleShot(hallOfFameMS, this, SLOT(end()));
        return;
    }

    lastPlayer = player;

    for (int i = 0; i < 10; i++)
        if ( playerScore[lastPlayer] > hallOfFame[i].points) {
            lastScore = i;
            break;
        }

    if (lastScore < 0) {
        emit toggleNew();
        QTimer::singleShot(hallOfFameMS, this, SLOT(end()));
        return;
    }

    for (int i = 9; i > lastScore && i > 0; i--)
        hallOfFame[i] = hallOfFame[i-1];

    hallOfFame[lastScore].points = playerScore[lastPlayer];
    hallOfFame[lastScore].levels = level;
    hallOfFame[lastScore].moment = QDateTime::currentDateTime();
    hallOfFame[lastScore].name = playerName[lastPlayer];

    cursor.x = 14;
    cursor.y = 11+lastScore;
    cursor.chr = hallOfFame[lastScore].name.at(cursor.x-14);

//  startTimer(cursorBlinkMS);
    setFocus();
}

/*
 * Read the highscores, if no file or a file shorter than 4 bytes (versions before 0.2.4 stores only
 * the points of one highscore) exists - the highscores were initialized with default values.
 */
void Score::read()
{
    if (highscoreFile.exists() && highscoreFile.size() > 4) {
        if (highscoreFile.open(IO_ReadOnly)) {
            QDataStream s(&highscoreFile);
            char *name;
            for (int i = 0; i < 10; i++) {
                s >> hallOfFame[i].points >> hallOfFame[i].levels >> hallOfFame[i].duration >>
                     hallOfFame[i].moment >> name;
                hallOfFame[i].name = QString::fromLatin1(name);
                delete(name);
            }
            highscoreFile.close();
        }
    } else {
        for (int i = 0; i < 10; i++) {
            hallOfFame[i].points = 5000;
            hallOfFame[i].levels = 0;
            hallOfFame[i].duration = QTime();
            hallOfFame[i].moment = QDateTime();
            hallOfFame[i].name = "???";
        }
        // write();
    }

    for (int i = 0; i < 10; i++)
        for (uint j = 0; j < hallOfFame[i].name.length(); j++)
            if (hallOfFame[i].name.at(j) < bitfont->firstChar() ||
                hallOfFame[i].name.at(j) > bitfont->lastChar())
                hallOfFame[i].name.at(j) = hallOfFame[i].name.at(j).upper();

    HighScore = hallOfFame[0].points;
}

void Score::write()
{
    if (highscoreFile.open(IO_WriteOnly)) {
        QDataStream s(&highscoreFile);
        for (int i = 0; i < 10; i++)
            s << hallOfFame[i].points << hallOfFame[i].levels << hallOfFame[i].duration <<
                 hallOfFame[i].moment << hallOfFame[i].name.latin1();
        highscoreFile.close();
    }
}

void Score::setPause(bool Paused)
{
    paused = Paused;

    QRect r = bitfont->rect(tr("PAUSED"));
    r.moveCenter(QPoint(this->width()/2, this->height()/2));
    repaint(r, TRUE);

    // repaint 1UP or 2UP
    repaint(FALSE);
}

void Score::end()
{
    if (paused) {
        QTimer::singleShot(afterPauseMS, this, SLOT(end()));
        return;
    }

    // repaint 1UP or 2UP
    lastPlayer = -1;
    repaint(FALSE);

    emit forcedHallOfFame(FALSE);
}

/*
 * Return the date in a formatted QString. The format can be changed using internationalization
 * of the string "YY/MM/DD". Invalid QDate's where returned as "00/00/00".
 */
QString Score::formatDate(QDate date)
{
    QString s = tr("@YY@/@MM@/@DD@");

    QString dd;
    dd.sprintf("%02d", date.isValid() ? date.year() % 100 : 0);
    s.replace(QRegExp("@YY@"), dd);
    dd.sprintf("%02d", date.isValid() ? date.month() : 0);
    s.replace(QRegExp("@MM@"), dd);
    dd.sprintf("%02d", date.isValid() ? date.day() : 0);
    s.replace(QRegExp("@DD@"), dd);

    return s;
}

QRect Score::rect(int col, float row, QString str, int align)
{
    QRect r = bitfont->rect(str);
    r.moveBy(x(col), y(row));

    int dx = 0;
    int dy = 0;

    if (align & AlignLeft || align & AlignRight) {
        dx = (str.length()-1) * (bitfont->width()/2);
        if (align & AlignRight)
            dx *= -1;
    }

    if (align & AlignTop || align & AlignBottom) {
        dy = bitfont->height()/2;
        if (align & AlignBottom)
            dy *= -1;
    }

    if (dx != 0 || dy != 0)
        r.moveBy(dx, dy);

    return r;
}

int Score::x(int col)
{
    return col*bitfont->width();
}

int Score::y(float row)
{
    return (int) (row*(bitfont->height()+bitfont->height()/4));
}

/**
 * Ermittelt die zu benutzende "highscore"-Datei, in die auch geschrieben werden kann.
 * Über den "highscoreFilePath"-KConfig-Eintrag, kann abweichend von der Standardlokation
 * der Standort der "highscore"-Datei spezifiziert werden.
 * Wenn die systemweite "highscore"-Datei nicht beschrieben werden kann, wird mit einer
 * privaten Datei gearbeitet.
 */
QFileInfo Score::locateHighscoreFilePath()
{
    return privateHighscoreFileInfo;
}
