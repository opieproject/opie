#ifndef SCORE_H
#define SCORE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "portable.h"

#if defined( KDE2_PORT )
#include <kapp.h>
#endif

#include <qwidget.h>
#include <qstring.h>
#include <qpoint.h>
#include <qrect.h>
#include <qfile.h>

#include <qfileinfo.h>
#include <qdatetime.h>

#include "painter.h"
#include "bitfont.h"

#define maxPlayer 1
#define minPlayerNameLength 3
#define highscoreName "highscore"

class Score : public QWidget
{
    Q_OBJECT
public:
    Score (QWidget *parent=0, const char *name=0, int scheme=-1, int mode=-1, Bitfont *font=0);
    ~Score();

public slots:
    void setScheme(int scheme, int mode, Bitfont *font=0);

    void set(int score);
    void set(int score, int player);
    void setScore(int level, int player);
    void setPause(bool paused);

    void initKeys();

private slots:
    void read();
    void write();
    void end();

signals:
    void toggleNew();
    void forcedHallOfFame(bool);

protected:
    void timerEvent(QTimerEvent *);
    void paintEvent(QPaintEvent *);
    void keyPressEvent(QKeyEvent *);

    void focusInEvent(QFocusEvent *) { ; }
    void focusOutEvent(QFocusEvent *) { ; }

    void confScheme();
    void confTiming(bool defGroup=TRUE);

private:
    Bitfont *bitfont;

    QRect rect(int col, float row, QString str, int align = AlignCenter);
    int x(int col);
    int y(float row);

    QString formatDate(QDate date);

		/**
		 * Ermittelt die zu benutzende "highscore"-Datei, in die auch geschrieben werden kann.
		 * Über den "highscoreFilePath"-KConfig-Eintrag, kann abweichend von der Standardlokation
		 * der Standort der "highscore"-Datei spezifiziert werden.
		 * Wenn die systemweite "globale" Datei nicht beschrieben werden kann, wird mit einer
		 * privaten Datei gearbeitet.
		 */
    QFileInfo locateHighscoreFilePath();

    int cursorBlinkMS;
    int hallOfFameMS;
    int afterPauseMS;

    bool paused;

    uint UpKey;
    uint DownKey;
    uint RightKey;
    uint LeftKey;

    int lastScore;
    int lastPlayer;
    int HighScore;
    int playerScore[maxPlayer];
    QString playerName[maxPlayer];

    struct {
        int x;
        int y;
        QChar chr;
        bool on;
    } cursor;

    int cursorBlinkTimer;
    bool scrollRepeat;

    struct {
        int points;
        int levels;
        QTime duration;
        QDateTime moment;
        QString name;
    } hallOfFame[10];

    QFileInfo systemHighscoreFileInfo;
    QFileInfo privateHighscoreFileInfo;

    QFile highscoreFile;

    int scheme;
    int mode;
};

#endif // SCORE_H
