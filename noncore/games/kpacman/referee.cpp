
#include "portable.h"

#if defined( KDE2_PORT )
#include <kapp.h>
#include <kconfig.h>
#include <kstddirs.h>
#include <kaccel.h>
#include <referee.h>
#include <referee.moc>
#elif defined( QPE_PORT )
#include <qaccel.h>
#include <qpe/qpeapplication.h>
#include "config.h"
#include "referee.h"
#endif

#include <qdatetm.h>
#include <stdlib.h>
#include <qtimer.h>
#include <qevent.h>
#include <qcolor.h>
#include <qkeycode.h>
#include <qfileinfo.h>

#include "board.h"
#include "pacman.h"
#include "monster.h"
#include "fruit.h"
#include "painter.h"

Referee::Referee( QWidget *parent, const char *name, int Scheme, int Mode, Bitfont *font)
    : QWidget( parent, name )
{
    gameState.resize(12);
    gameTimer = 0;
    energizerTimer = 0;

    focusedPause = FALSE;
    setFocusPolicy(QWidget::StrongFocus);

    initKeys();

    scheme = Scheme;
    mode = Mode;
    confScheme();

    board = new Board(BoardWidth*BoardHeight);

    pix = new Painter(board, this, scheme, mode, font);
    setFixedSize(pix->levelPix().size());

    pacman = new Pacman(board);

    fruit = new Fruit(board);

    monsters = new QList<Monster>;
    monsters->setAutoDelete(TRUE);

    monsterRect = new QList<QRect>;
    monsterRect->setAutoDelete(TRUE);

    energizers = new QList<Energizer>;
    energizers->setAutoDelete(TRUE);

    energizerRect = new QList<QRect>;
    energizerRect->setAutoDelete(TRUE);

    pacmanRect.setRect(0, 0, 0, 0);
    fruitRect.setRect(0, 0, 0, 0);

    QTime midnight( 0, 0, 0 );
    srand( midnight.secsTo(QTime::currentTime()) );

    lifes = 0;
    points = 0;

    emit setLifes(lifes);
    emit setPoints(points);

    intro();
}

void Referee::paintEvent( QPaintEvent *e)
{
    if (gameState.testBit(HallOfFame))
        return;

    QRect rect = e->rect();

    if (!rect.isEmpty()) {
        QPixmap p = pix->levelPix();
        bitBlt(this, rect.x(), rect.y(),
               &p, rect.x(), rect.y(), rect.width(), rect.height());
    }

    if ((gameState.testBit(GameOver) || gameState.testBit(Demonstration)) &&
        rect.intersects(pix->rect(board->position(fruithome), tr("GAME  OVER"))))
        pix->draw(board->position(fruithome), Widget, tr("GAME  OVER"), RED);

    for (Energizer *e = energizers->first(); e != 0; e = energizers->next()) {
        if (e && e->state() == on &&
            rect.intersects(pix->rect(e->position(), EnergizerPix)) &&
            !(e->position() == pacman->position() && gameState.testBit(Scoring))) {
            if (e->pix() != -1)
                pix->draw(e->position(), Widget, EnergizerPix, e->pix());
        }
    }

    if (!gameState.testBit(Init)) {

        if (!gameState.testBit(Dying) && (fruit->pix() != -1))
            if (fruit->state() != active) {
                if (rect.intersects(pix->rect(fruit->position(), FruitScorePix, fruit->pix())))
                    pix->draw(fruit->position(), Widget, FruitScorePix, fruit->pix());
            } else {
                if (rect.intersects(pix->rect(fruit->position(), FruitPix, fruit->pix())))
                    pix->draw(fruit->position(), Widget, FruitPix, fruit->pix());
            }

        for (Monster *m = monsters->first(); m != 0; m = monsters->next())
            if (m && m->state() == harmless &&
                rect.intersects(pix->rect(m->position(), MonsterPix)) &&
                !(m->position() == pacman->position() && gameState.testBit(Scoring))) {
                if (m->body() != -1)
                    pix->draw(m->position(), Widget, MonsterPix, m->body());
                if (m->eyes() != -1)
                    pix->draw(m->position(), Widget, EyesPix, m->eyes());
            }

        if (!gameState.testBit(Scoring) && !gameState.testBit(LevelDone) &&
            rect.intersects(pix->rect(pacman->position(), PacmanPix)) && pacman->pix() != -1)
            pix->draw(pacman->position(), Widget, PacmanPix, pacman->pix());

        for (Monster *m = monsters->first(); m != 0; m = monsters->next())
            if (m && m->state() != harmless &&
                rect.intersects(pix->rect(m->position(), MonsterPix)) &&
                !(m->position() == pacman->position() && gameState.testBit(Scoring))) {
                if (m->body() != -1)
                    pix->draw(m->position(), Widget, MonsterPix, m->body());
                if (m->eyes() != -1)
                    pix->draw(m->position(), Widget, EyesPix, m->eyes());
            }
    }

    if (gameState.testBit(Scoring) &&
        rect.intersects(pix->rect(pacman->position(), MonsterScorePix, monstersEaten-1)))
        pix->draw(pacman->position(), Widget, MonsterScorePix, monstersEaten-1);

    if (gameState.testBit(Init) && gameState.testBit(Dying) &&
        timerCount < pix->maxPixmaps(DyingPix) &&
        rect.intersects(pix->rect(pacman->position(), PacmanPix)))
        pix->draw(pacman->position(), Widget, DyingPix, timerCount);

    if (gameState.testBit(LevelDone) &&
        rect.intersects(pix->rect(pacman->position(), PacmanPix)))
        pix->draw(pacman->position(), Widget, PacmanPix, pacman->pix());

    if (gameState.testBit(Player) &&
        rect.intersects(pix->rect(board->position(monsterhome, 0), tr("PLAYER ONE"))))
        pix->draw(board->position(monsterhome, 0), Widget, tr("PLAYER ONE"), CYAN);

    if (gameState.testBit(Ready) &&
        rect.intersects(pix->rect(board->position(fruithome), tr("READY!"))))
        pix->draw(board->position(fruithome), Widget, tr("READY!"), YELLOW);

    if (gameState.testBit(Paused) &&
        rect.intersects(pix->rect((BoardWidth*BoardHeight)/2-BoardWidth, tr("PAUSED"))))
        pix->draw((BoardWidth*BoardHeight)/2-BoardWidth, Widget, tr("PAUSED"), RED, BLACK);
}

void Referee::timerEvent( QTimerEvent *e )
{
    if (gameState.testBit(HallOfFame))
        return;

    QRect lastRect;
    int lastPix;
    bool moved = FALSE;
    int eated = 0;

    if (e->timerId() == energizerTimer) {
        for (int e = 0; e < board->energizers(); e++) {
            lastRect = pix->rect(energizers->at(e)->position(), EnergizerPix);
            lastPix = energizers->at(e)->pix();
            if (energizers->at(e)->move()) {
                moved = TRUE;
                *energizerRect->at(e) = pix->rect(energizers->at(e)->position(), EnergizerPix);
                if (lastPix == energizers->at(e)->pix() &&
                    lastRect == pix->rect(energizers->at(e)->position(), EnergizerPix))
                    energizerRect->at(e)->setRect(0, 0, 0, 0);
                else
                    *energizerRect->at(e) = pix->rect(*energizerRect->at(e), lastRect);
            }
        }

        for (int e = 0; e < board->energizers(); e++)
            if (!energizerRect->at(e)->isNull())
                repaint(*energizerRect->at(e), FALSE);

        return;
    }

    timerCount++;

    lastRect = pix->rect(pacman->position(), PacmanPix);
    lastPix = pacman->pix();

    if (moved = pacman->move()) {       // pacman really moved
        pacmanRect = pix->rect(pacman->position(), PacmanPix);
        if (lastPix == pacman->pix() &&
            lastRect == pix->rect(pacman->position(), PacmanPix))
            pacmanRect.setRect(0, 0, 0, 0);     // nothing to do, because the pixmap
        else                                    // and the position isn't changed.
            pacmanRect = pix->rect(pacmanRect, lastRect);
    } else
        pacmanRect.setRect(0, 0, 0, 0);

    int pos = pacman->position();

    if (moved && board->isMonster(pos) && !gameState.testBit(Dying)) {
        for (Monster *m = monsters->first(); m != 0; m = monsters->next())
            if (m  && m->position() == pos) {
                if (m->state() == harmless && !gameState.testBit(Dying)) {
                    m->setREM(remTicks[level]);
                    m->setDirection(X); // prevent movement before eaten()
                    eated++;
                    if (gameState.testBit(Introducing))
                        m->setPosition(OUT);
                }
                if (m->state() == dangerous && !gameState.testBit(Dying))
                    killed();
            }
    }

    if (moved && !gameState.testBit(Dying)) {
        if (board->isPoint(pos)) {
            board->reset(pos, Point);
            score(pointScore);
            pix->erase(pos, PointPix);
        }
        if (board->isEnergizer(pos)) {
            for (int e = 0; e < board->energizers();e++) {
                if (energizers->at(e)->position() == pos) {
                    energizers->at(e)->setOff();
                  energizers->remove(e);
                  energizerRect->remove(e);
                  e = board->energizers();
                }
            }
            board->reset(pos, energizer);
            score(energizerScore);
            monstersEaten = 0;
            for (Monster *m = monsters->first(); m != 0; m = monsters->next())
                if (m && m->state() != rem) {
                    m->setHarmless(harmlessTicks[level], harmlessDurTicks[level],
                                   harmlessWarnTicks[level]);
                    if (gameState.testBit(Introducing))
                        m->setDirection(board->turn(m->direction()));
                }
        }
        if (pos == fruit->position() && fruit->state() == active) {
            fruit->setEaten(fruitScoreDurTicks[level]);
            initFruit(FALSE);
            score(fruitScore[fruit->pix()]);
        }
    }

    if (!gameState.testBit(Introducing)) {
        if (fruit->state() != active && fruit->pix() >= 0)
            lastRect = pix->rect(fruit->position(), FruitScorePix, fruit->pix());
        else
            lastRect = pix->rect(fruit->position(), FruitPix, fruit->pix());

        lastPix = fruit->pix();
        if (fruit->move()) {
            if (pos == fruit->position() && fruit->state() == active) {
                fruit->setEaten(fruitScoreDurTicks[level]);
                initFruit(FALSE);
                score(fruitScore[fruit->pix()]);
            }
            if (fruit->state() != active && fruit->pix() >= 0)
                fruitRect = pix->rect(fruit->position(), FruitScorePix, fruit->pix());
            else
                fruitRect = pix->rect(fruit->position(), FruitPix, fruit->pix());
            if (lastPix == fruit->pix() && lastRect == fruitRect)
                fruitRect.setRect(0, 0, 0, 0);
            else
                fruitRect = pix->rect(fruitRect, lastRect);
        } else
            fruitRect.setRect(0, 0, 0, 0);
    } else
        fruitRect.setRect(0, 0, 0, 0);

    int lastBodyPix;
    int lastEyesPix;

    for (Monster *m = monsters->first(); m != 0; m = monsters->next())
        if (m) {
            lastRect = pix->rect(m->position(), MonsterPix);
            lastBodyPix = m->body();
            lastEyesPix = m->eyes();
            if (m->move()) {
                moved = TRUE;
                *monsterRect->at(m->id()) = pix->rect(m->position(), MonsterPix);
                if (lastBodyPix == m->body() && lastEyesPix == m->eyes() &&
                    lastRect == pix->rect(m->position(), MonsterPix))
                    monsterRect->at(m->id())->setRect(0, 0, 0, 0);
                else
                    *monsterRect->at(m->id()) = pix->rect(*monsterRect->at(m->id()), lastRect);
                if (m->position() == pos && !gameState.testBit(Dying)) {
                    if (m->state() == harmless && !gameState.testBit(Dying)) {
                        m->setREM(remTicks[level]);
                        eated++;
                        if (gameState.testBit(Introducing)) {
                            m->setPosition(OUT);
                            m->setDirection(X);
                        }
                    }
                    if (m->state() == dangerous && !gameState.testBit(Dying))
                        killed();
                }
            } else
                monsterRect->at(m->id())->setRect(0, 0, 0, 0);
        }

    for (int m = 0; m < board->monsters(); m++)
        if (pacmanRect.intersects(*monsterRect->at(m))) {
            pacmanRect = pix->rect(pacmanRect, *monsterRect->at(m));
            monsterRect->at(m)->setRect(0, 0, 0, 0);
        } else
            for (int im = m+1; im < board->monsters(); im++)
                if (monsterRect->at(m)->intersects(*monsterRect->at(im))) {
                    *monsterRect->at(m) = pix->rect(*monsterRect->at(m), *monsterRect->at(im));
                    monsterRect->at(im)->setRect(0, 0, 0, 0);
                }

    if (!pacmanRect.isNull())
        repaint(pacmanRect, FALSE);

    if (!fruitRect.isNull())
        repaint(fruitRect, FALSE);

    for (int m = 0; m < board->monsters(); m++)
        if (!monsterRect->at(m)->isNull())
            repaint(*monsterRect->at(m), FALSE);

    if (board->points() == 0 && !gameState.testBit(Dying))
        levelUp();

    if (eated > 0 && !gameState.testBit(Dying)) {
        timerCount = eated;
        eaten();
    }

    if (gameState.testBit(Introducing) && moved)
        introPlay();
}

void Referee::repaintFigures()
{
    pacmanRect = pix->rect(pacman->position(), PacmanPix);

    for (int e = 0; e < board->energizers(); e++) {
        *energizerRect->at(e) = pix->rect(board->position(energizer, e), EnergizerPix);

        if (pacmanRect.intersects(*energizerRect->at(e))) {
            pacmanRect = pix->rect(pacmanRect, *energizerRect->at(e));
            energizerRect->at(e)->setRect(0, 0, 0, 0);
        } else
            for (int ie = e+1; ie < board->energizers(); ie++)
                if (energizerRect->at(e)->intersects(*energizerRect->at(ie))) {
                    *energizerRect->at(e) = pix->rect(*energizerRect->at(e), *energizerRect->at(ie));
                    energizerRect->at(ie)->setRect(0, 0, 0, 0);
                }
    }

    if (fruit->pix() != -1 && fruit->state() != active)
        fruitRect = pix->rect(fruit->position(), FruitScorePix, fruit->pix());
    else
        fruitRect = pix->rect(fruit->position(), FruitPix, fruit->pix());

    if (pacmanRect.intersects(fruitRect)) {
        pacmanRect = pix->rect(pacmanRect, fruitRect);
        fruitRect.setRect(0, 0, 0, 0);
    }

    for (int m = 0; m < board->monsters(); m++) {
        *monsterRect->at(m) = pix->rect(board->position(monster, m), MonsterPix);

        if (pacmanRect.intersects(*monsterRect->at(m))) {
            pacmanRect = pix->rect(pacmanRect, *monsterRect->at(m));
            monsterRect->at(m)->setRect(0, 0, 0, 0);
        } else
            for (int im = m+1; im < board->monsters(); im++)
                if (monsterRect->at(m)->intersects(*monsterRect->at(im))) {
                    *monsterRect->at(m) = pix->rect(*monsterRect->at(m), *monsterRect->at(im));
                    monsterRect->at(im)->setRect(0, 0, 0, 0);
                }
    }

    if (!pacmanRect.isNull())
        repaint(pacmanRect, FALSE);

    if (!fruitRect.isNull())
        repaint(fruitRect, FALSE);

    for (int m = 0; m < board->monsters(); m++)
        if (!monsterRect->at(m)->isNull())
            repaint(*monsterRect->at(m), FALSE);

    for (int e = 0; e < board->energizers(); e++)
        if (!energizerRect->at(e)->isNull())
            repaint(*energizerRect->at(e), FALSE);

}

void Referee::initKeys()
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

void Referee::fillArray(QArray<int> &array, QString values, int max)
{
    if (max < 0)
        max = values.contains(',')+1;

    array.resize(max);
    int last = 0;
    bool ok;
    QString value;

    for (uint i = 0; i < array.size(); i++) {
        if (values.find(',') < 0 &&  values.length() > 0) {
            value = values;
            values = "";
        }
        if (values.find(',') >= 0) {
            value = values.left(values.find(','));
            values.remove(0,values.find(',')+1);
        }
        array[i] = value.toInt(&ok);
        if (ok)
            last = array[i];
        else
            array[i] = last;
    }
}

void Referee::fillStrList(QStrList &list, QString values, int max)
{
    if (!list.isEmpty())
        list.clear();

    QString last = "";
    QString value;

    for (uint i = 0; i < (uint) max; i++) {
        if (values.find(',') < 0 &&  values.length() > 0) {
            value = values;
            values = "";
        }
        if (values.find(',') >= 0) {
            value = values.left(values.find(','));
            values.remove(0,values.find(',')+1);
        }
        if (!value.isEmpty())
            last = value;

        list.append(last);
    }
}

void Referee::fillMapName()
{
    QStrList list = mapName;

    if (!mapName.isEmpty())
                                mapName.clear();

    QString map;

    QFileInfo fileInfo;

    for (uint i = 0; i < list.count(); i++) {
        map = list.at(i);

        if (map.left(1) != "/" && map.left(1) != "~")
                  map = FIND_APP_DATA( mapDirectory+map );

        fileInfo.setFile(map);
        if (!fileInfo.isReadable())
            map = "";
        
        mapName.append(map);
    }
}

void Referee::confLevels(bool defGroup)
{
    APP_CONFIG_BEGIN( cfg );
    if (defGroup || cfg->hasKey("Levels"))
        maxLevel = cfg->readNumEntry("Levels", 13);
    APP_CONFIG_END( cfg );
}

void Referee::confMisc(bool defGroup)
{
    APP_CONFIG_BEGIN( cfg );
    if (defGroup || cfg->hasKey("PixmapDirectory")) {
        pixmapDirectory = cfg->readEntry("PixmapDirectory");

        if (pixmapDirectory.left(1) != "/" && pixmapDirectory.left(1) != "~")
                  pixmapDirectory.insert(0, "pics/");
        if (pixmapDirectory.right(1) != "/")
                  pixmapDirectory.append("/");
    }

    if (defGroup || cfg->hasKey("MapDirectory")) {
        mapDirectory = cfg->readEntry("MapDirectory");

        if (mapDirectory.left(1) != "/" && mapDirectory.left(1) != "~")
            mapDirectory.insert(0, "maps/");
        if (mapDirectory.right(1) != "/")
            mapDirectory.append("/");
    }

    if (defGroup || cfg->hasKey("MapName"))
        fillStrList(mapName, cfg->readEntry("MapName", "map"), maxLevel+1);

    if (defGroup || cfg->hasKey("MonsterIQ"))
        fillArray(monsterIQ, cfg->readEntry("MonsterIQ", "0,170,180,170,180,170,180"), maxLevel+1);
    if (defGroup || cfg->hasKey("FruitIQ"))
        fillArray(fruitIQ, cfg->readEntry("FruitIQ", "0,170,180,170,180,170,180"), maxLevel+1);
    if (defGroup || cfg->hasKey("FruitIndex"))
        fillArray(fruitIndex, cfg->readEntry("FruitIndex", "0"), maxLevel+1);
    APP_CONFIG_END( cfg );
}

void Referee::confTiming(bool defGroup)
{
    APP_CONFIG_BEGIN( cfg );
    if (defGroup || cfg->hasKey("SpeedMS"))
        fillArray(speed, cfg->readEntry("SpeedMS", "20"), maxLevel+1);
    if (defGroup || cfg->hasKey("PacmanTicks"))
        fillArray(pacmanTicks,cfg->readEntry("PacmanTicks", "3"), maxLevel+1);
    if (defGroup || cfg->hasKey("RemTicks"))
        fillArray(remTicks, cfg->readEntry("RemTicks", "1"), maxLevel+1);
    if (defGroup || cfg->hasKey("DangerousTicks"))
        fillArray(dangerousTicks, cfg->readEntry("DangerousTicks", "3"), maxLevel+1);
    if (defGroup || cfg->hasKey("HarmlessTicks"))
        fillArray(harmlessTicks, cfg->readEntry("HarmlessTicks", "7,6,,5,,4"), maxLevel+1);
    if (defGroup || cfg->hasKey("HarmlessDurationTicks"))
        fillArray(harmlessDurTicks, cfg->readEntry("HarmlessDurationTicks", "375,,,300,,250,200,150"), maxLevel+1);
    if (defGroup || cfg->hasKey("HarmlessWarningTicks"))
        fillArray(harmlessWarnTicks, cfg->readEntry("HarmlessWarningTicks", "135"), maxLevel+1);
    if (defGroup || cfg->hasKey("ArrestTicks"))
        fillArray(arrestTicks, cfg->readEntry("ArrestTicks", "6"), maxLevel+1);
    if (defGroup || cfg->hasKey("ArrestDurationTicks"))
        fillArray(arrestDurTicks, cfg->readEntry("ArrestDurationTicks", "200,,,150"), maxLevel+1);
    if (defGroup || cfg->hasKey("FruitTicks"))
        fillArray(fruitTicks, cfg->readEntry("FruitTicks", "7,6,,5,,4"), maxLevel+1);
    if (defGroup || cfg->hasKey("FruitAppearsTicks"))
        fillArray(fruitAppearsTicks, cfg->readEntry("FruitAppearsTicks", "1000,,1500,2000,2500,3000,3500,4000"), maxLevel+1);
    if (defGroup || cfg->hasKey("FruitDurationTicks"))
        fillArray(fruitDurTicks, cfg->readEntry("FruitDurationTicks", "500,,,400,350,300,,250,200,150"), maxLevel+1);
    if (defGroup || cfg->hasKey("FruitScoreDurationTicks"))
        fillArray(fruitScoreDurTicks, cfg->readEntry("FruitScoreDurationTicks", "150"), maxLevel+1);

    if (defGroup || cfg->hasKey("MonsterScoreDurationMS"))
        monsterScoreDurMS = cfg->readNumEntry("MonsterScoreDurationMS", 1000);
    if (defGroup || cfg->hasKey("PlayerDurationMS"))
        playerDurMS = cfg->readNumEntry("PlayerDurationMS", 3000);
    if (defGroup || cfg->hasKey("ReadyDurationMS"))
        readyDurMS = cfg->readNumEntry("ReadyDurationMS", 2000);
    if (defGroup || cfg->hasKey("GameOverDurationMS"))
        gameOverDurMS = cfg->readNumEntry("GameOverDurationMS", 3000);
    if (defGroup || cfg->hasKey("AfterPauseMS"))
        afterPauseMS = cfg->readNumEntry("AfterPauseMS", 1000);
    if (defGroup || cfg->hasKey("DyingPreAnimationMS"))
        dyingPreAnimationMS = cfg->readNumEntry("DyingPreAnimationMS", 1000);
    if (defGroup || cfg->hasKey("DyingAnimationMS"))
        dyingAnimationMS = cfg->readNumEntry("DyingAnimationMS", 100);
    if (defGroup || cfg->hasKey("DyingPostAnimationMS"))
        dyingPostAnimationMS = cfg->readNumEntry("DyingPostAnimationMS", 500);
    if (defGroup || cfg->hasKey("IntroAnimationMS"))
        introAnimationMS = cfg->readNumEntry("IntroAnimationMS", 800);
    if (defGroup || cfg->hasKey("IntroPostAnimationMS"))
        introPostAnimationMS = cfg->readNumEntry("IntroPostAnimationMS", 1000);
    if (defGroup || cfg->hasKey("LevelUpPreAnimationMS"))
        levelUpPreAnimationMS = cfg->readNumEntry("LevelUpPreAnimationMS", 2000);
    if (defGroup || cfg->hasKey("LevelUpAnimationMS"))
        levelUpAnimationMS = cfg->readNumEntry("LevelUpAnimationMS", 2000);
    if (defGroup || cfg->hasKey("EnergizerAnimationMS"))
        energizerAnimationMS = cfg->readNumEntry("EnergizerAnimationMS", 200);
    APP_CONFIG_END( cfg );
}

void Referee::confScoring(bool defGroup)
{
    APP_CONFIG_BEGIN( cfg );
    if (defGroup || cfg->hasKey("PointScore"))
        pointScore = cfg->readNumEntry("PointScore", 10);
    if (defGroup || cfg->hasKey("EnergizerScore"))
        energizerScore = cfg->readNumEntry("EnergizerScore", 50);
    if (defGroup || cfg->hasKey("FruitScore"))
        fillArray(fruitScore, cfg->readEntry("FruitScore", "100,300,500,,700,,1000,,2000,,3000,,5000"), maxLevel+1);
    if (defGroup || cfg->hasKey("MonsterScore"))
        fillArray(monsterScore, cfg->readEntry("MonsterScore", "200,400,800,1600"), 4);
    if (defGroup || cfg->hasKey("ExtraLifeScore"))
        fillArray(extraLifeScore, cfg->readEntry("ExtraLifeScore", "10000"), -1);
    APP_CONFIG_END( cfg );
}

void Referee::confScheme()
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

    confLevels();

    if (mode != -1) {
        newgroup.sprintf("Mode %d", mode);
        cfg->setGroup(newgroup);

        confLevels(FALSE);
    }

    if (scheme != -1) {
        newgroup.sprintf("Scheme %d", scheme);
        cfg->setGroup(newgroup);

        confLevels(FALSE);
    }

    RESTORE_CONFIG_GROUP( cfg, oldgroup );

    confMisc();
    confTiming();
    confScoring();

    if (mode != -1) {
        newgroup.sprintf("Mode %d", mode);
        cfg->setGroup(newgroup);

        confMisc(FALSE);
        confTiming(FALSE);
        confScoring(FALSE);
    }

    if (scheme != -1) {
        newgroup.sprintf("Scheme %d", scheme);
        cfg->setGroup(newgroup);

        confMisc(FALSE);
        confTiming(FALSE);
        confScoring(FALSE);
    }

    fillMapName();

    RESTORE_CONFIG_GROUP( cfg, oldgroup );
    APP_CONFIG_END( cfg );
}

void Referee::setScheme(int Scheme, int Mode, Bitfont *font)
{
    mode = Mode;
    scheme = Scheme;

    confScheme();

    pix->setScheme(scheme, mode, font);

    pacman->setMaxPixmaps(pix->maxPixmaps(PacmanPix));
    fruit->setMaxPixmaps(pix->maxPixmaps(FruitPix));

    for (Monster *m = monsters->first(); m != 0; m = monsters->next())
        if (m)
            m->setMaxPixmaps(pix->maxPixmaps(MonsterPix), pix->maxPixmaps(EyesPix));

    for (Energizer *e = energizers->first(); e != 0; e = energizers->next())
        if (e)
            e->setMaxPixmaps(pix->maxPixmaps(EnergizerPix));

    if (gameState.testBit(Introducing))
        for (int i = 0; i < (gameState.testBit(Init) ? timerCount : 15); i++)
            introPaint(i);

    setFixedSize(pix->levelPix().size());
    repaint();
}

void Referee::keyPressEvent( QKeyEvent *k )
{
    uint key = k->key();
#ifdef QWS
    // "OK" => new game
    if ( key == Key_F33 || key == Key_F2 || key == Key_Enter )
        play();
    else if ( !gameState.testBit(Playing) &&
						( key == Key_Up ||
            			key == Key_Down ||
            			key == Key_Right ||
            			key == Key_Left )) 
        play();
    // "Space" => pause
    //else if ( key == Key_Space || key == Key_F3 )
    //    pause();
    // "Menu" => hall of fame
    //else if ( key == Key_F11 || key == Key_F4 )
    //    toggleHallOfFame();
#endif

    if (gameState.testBit(Paused) || gameState.testBit(HallOfFame) ||
        gameState.testBit(Demonstration) || gameState.testBit(Dying) ||
        gameState.testBit(Ready) || gameState.testBit(LevelDone) ||
        !gameState.testBit(Playing))
        return;

    if (key == UpKey)
        pacman->setDirection(N);
    else if (key == DownKey)
        pacman->setDirection(S);
    else if (key == RightKey)
        pacman->setDirection(E);
    else if (key == LeftKey)
        pacman->setDirection(W);

#ifdef CHEATS
    else if (key == Key_L) { printf("levelUp()\n"); levelUp(); }
    else if (key == Key_F) { printf("fruit->move(TRUE)\n"); fruit->move(TRUE); repaint(FALSE); }
    else if (key == Key_E) { printf("setLifes(++lifes)\n"); emit setLifes(++lifes); }
#endif

    else {
        k->ignore();
        return;
    }
    k->accept();
}

void Referee::score(int p)
{
    if (!gameState.testBit(Playing))
        return;

    if ((points += p) < 0)
        points = 0;

    emit setPoints(points);

    if (points >= nextExtraLifeScore) {
        emit setLifes(++lifes);
        if (extraLifeScoreIndex < (int) extraLifeScore.size()-1)
            extraLifeScoreIndex++;
        if (extraLifeScore[extraLifeScoreIndex] < 0)
            nextExtraLifeScore = extraLifeScore[extraLifeScoreIndex] * -1;
        else
            nextExtraLifeScore += extraLifeScore[extraLifeScoreIndex];
    }
}

void Referee::eaten()
{
    if (gameState.testBit(Ready))
        return;

    stop();

    if (monstersEaten < 4)
        monstersEaten++;

    gameState.setBit(Scoring);
    score(monsterScore[monstersEaten-1]);

    repaint(pix->rect(pix->rect(pacman->position(), MonsterPix),
                      pix->rect(pacman->position(), MonsterScorePix, monstersEaten-1)));

    if (--timerCount > 0)
        QTimer::singleShot( monsterScoreDurMS, this, SLOT(eaten()));
    else {
        for (Monster *m = monsters->first(); m != 0; m = monsters->next())
            if (m && m->direction() == X && !gameState.testBit(Introducing))
                m->setDirection(N);
        if (monstersEaten != 4 || !gameState.testBit(Introducing))
            QTimer::singleShot( monsterScoreDurMS, this, SLOT(start()));
    }
}

void Referee::toggleHallOfFame()
{
    gameState.toggleBit(HallOfFame);
}

void Referee::hallOfFame()
{
    if (gameState.testBit(HallOfFame))  // If the HallOfFame is switched on manually, toggle the
        toggleHallOfFame();             // bit twice.

    emit setLevel(0);                   // Clear status display for hall of fame
    emit setScore(level, 0);
    emit forcedHallOfFame(TRUE);
}

void Referee::pause()
{
    static int pausedTimer = 0;

    if (!gameState.testBit(Paused)) {
        pausedTimer = gameTimer;
        stop();
        stopEnergizer();
        gameState.setBit(Paused);
        repaint(pix->rect((BoardWidth*BoardHeight)/2-BoardWidth, tr("PAUSED")), FALSE);
    } else {
        gameState.clearBit(Paused);
        repaint(pix->rect((BoardWidth*BoardHeight)/2-BoardWidth, tr("PAUSED")), FALSE);
        if (pausedTimer) {
            pausedTimer = 0;
            start();
        }
    }
    emit togglePaused();
}

void Referee::intro()
{
    stop();
    stopEnergizer();
    bool paused = gameState.testBit(Paused);

    gameState.fill(FALSE);
    gameState.setBit(Introducing);
    gameState.setBit(Init);

    if (paused)
        gameState.setBit(Paused);

    level = 0;
    emit setLevel(level);

    board->init(Intro);
    pix->setLevel(level);

    initPacman();
    initFruit();
    initMonsters();
    initEnergizers();

    repaint();

    monstersEaten = 0;
    timerCount = 0;
    introPlay();
}

void Referee::introMonster(int id)
{
    Monster *m = new Monster(board, id);

    m->setPosition((10+id*6)*BoardWidth+10);
    m->setDirection(E);
    m->setDangerous(dangerousTicks[level], monsterIQ[level]);
    m->setMaxPixmaps(pix->maxPixmaps(MonsterPix), pix->maxPixmaps(EyesPix));

    if (m->body() != -1)
        pix->draw(m->position(), RoomPix, MonsterPix, m->body());
    if (m->eyes() != -1)
        pix->draw(m->position(), RoomPix, EyesPix, m->eyes());

    repaint(pix->rect(m->position(), MonsterPix), FALSE);
    m->setPosition(OUT);
}

void Referee::introPaint(int t)
{
    QString pts;

    switch (t) {
        case  0 : repaint(pix->draw(16, 6, RoomPix, tr("CHARACTER"), WHITE, QColor(), AlignLeft), FALSE);
                  repaint(pix->draw(36, 6, RoomPix, tr("/"), WHITE, QColor(), AlignLeft), FALSE);
                  repaint(pix->draw(40, 6, RoomPix, tr("NICKNAME"), WHITE, QColor(), AlignLeft), FALSE);
                  break;
        case  1 : introMonster(0);
                  break;
        case  2 : repaint(pix->draw(16, 10, RoomPix, tr("-SHADOW"), RED, QColor(), AlignLeft), FALSE);
                  break;
        case  3 : repaint(pix->draw(38, 10, RoomPix, tr("\"BLINKY\""), RED, QColor(), AlignLeft), FALSE);
                  break;
        case  4 : introMonster(1);
                  break;
        case  5 : repaint(pix->draw(16, 16, RoomPix, tr("-SPEEDY"), PINK, QColor(), AlignLeft), FALSE);
                  break;
        case  6 : repaint(pix->draw(38, 16, RoomPix, tr("\"PINKY\""), PINK, QColor(), AlignLeft), FALSE);
                  break;
        case  7 : introMonster(2);
                  break;
        case  8 : repaint(pix->draw(16, 22, RoomPix, tr("-BASHFUL"), CYAN, QColor(), AlignLeft), FALSE);
                  break;
        case  9 : repaint(pix->draw(38, 22, RoomPix, tr("\"INKY\""), CYAN, QColor(), AlignLeft), FALSE);
                  break;
        case 10 : introMonster(3);
                  break;
        case 11 : repaint(pix->draw(16, 28, RoomPix, tr("-POKEY"), ORANGE, QColor(), AlignLeft), FALSE);
                  break;
        case 12 : repaint(pix->draw(38, 28, RoomPix, tr("\"CLYDE\""), ORANGE, QColor(), AlignLeft), FALSE);
                  break;
        case 13 : pts.sprintf("%d", pointScore);
                  repaint(pix->draw(28, 44, RoomPix, pts.data(), WHITE, QColor(), AlignRight), FALSE);
                  repaint(pix->draw(31, 44, RoomPix, "\x1C\x1D\x1E", WHITE, QColor(), AlignLeft), FALSE);
                  pts.sprintf("%d", energizerScore);
                  repaint(pix->draw(28, 48, RoomPix, pts.data(), WHITE, QColor(), AlignRight), FALSE);
                  repaint(pix->draw(31, 48, RoomPix, "\x1C\x1D\x1E", WHITE, QColor(), AlignLeft), FALSE);
                  break;
        case 14 : // "@ 1980 MIDWAY MFG.CO."
#if defined( KDE2_PORT )
                  repaint(pix->draw(30, 58, RoomPix, "© 1998-2002 J.THÖNNISSEN", PINK), FALSE);
#elif defined( QPE_PORT )
                  repaint(pix->draw(30, 55, RoomPix, "© 1998-2002 J.THÖNNISSEN", PINK), FALSE);
                  repaint(pix->draw(29, 58, RoomPix, tr("QTOPIA PORT: CATALIN CLIMOV"), GREEN), FALSE);
                  repaint(pix->draw(29, 61, RoomPix, tr("PRESS CURSOR TO START"), GREEN), FALSE);
#endif
                  break;
    }
}

void Referee::introPlay()
{
    if (!gameState.testBit(Introducing) || gameState.testBit(Ready))
        return;
    if (gameState.testBit(Paused) || gameState.testBit(HallOfFame)) {
        QTimer::singleShot(afterPauseMS, this, SLOT(introPlay()));
        return;
    }

    if (!gameState.testBit(Init)) {
        if (monstersEaten == 4) {
            stop();
            QTimer::singleShot(introPostAnimationMS, this, SLOT(demo()));
        }
        if (pacman->direction() == W) {
            int id = -1;
            if (pacman->position() == 37*BoardWidth-6)
                id = 0;
            else
                if (board->isMonster(37*BoardWidth-6))
                    for (Monster *m = monsters->first(); m != 0; m = monsters->next())
                        if (m && m->position() == 37*BoardWidth-6) {
                            id = m->id();
                            id++;
                            break;
                        }
                
            if (id >= 0 && id <= 4)
                for (Monster *m = monsters->first(); m != 0; m = monsters->next())
                    if (m && m->id() == id && m->position() == OUT) {
                        m->setPosition(37*BoardWidth-1);
                        m->setDirection(W);
                        m->setDangerous(dangerousTicks[level], monsterIQ[level]);
                        board->set(37*BoardWidth-1, monsterhome, id);
                        repaint(pix->rect(m->position(), MonsterPix));
                        break;
                    }
        }
        return;
    }

    if (timerCount < 15)
        introPaint(timerCount);

    switch (timerCount) {
        case 13 : board->set(44*BoardWidth+22, Point);
                  pix->drawPoint(44*BoardWidth+22);
                  repaint(pix->rect(44*BoardWidth+22, PointPix), FALSE);
                  energizers->at(0)->setPosition(48*BoardWidth+22);
                  energizers->at(0)->setOn();
                  repaint(pix->rect(48*BoardWidth+22, EnergizerPix), FALSE);
                  break;
        case 14 : energizers->at(1)->setPosition(36*BoardWidth+10);
                  energizers->at(1)->setOn();
                  repaint(pix->rect(36*BoardWidth+10, EnergizerPix), FALSE);
                  for (int pos = 8; pos < BoardWidth; pos++) {
                      board->set(34*BoardWidth+pos, out);
                      board->set(38*BoardWidth+pos, out);
                  }
                  board->set(36*BoardWidth+8, out);
                  break;
        case 15 : gameState.clearBit(Init);
                  initPacman();
                  pacman->setDemo(TRUE);
                  pacman->setPosition(37*BoardWidth-1);
                  repaintFigures();
                  start();
                  return;               
    }

    if (timerCount++ < 15)
        QTimer::singleShot(introAnimationMS, this, SLOT(introPlay()));
}

void Referee::demo()
{
    if (gameState.testBit(Ready))
        return;

    if (gameState.testBit(Paused) || gameState.testBit(HallOfFame)) {   
        QTimer::singleShot(afterPauseMS, this, SLOT(demo()));
        return;
    }

    stop();
    stopEnergizer();

    gameState.fill(FALSE);
    gameState.setBit(Init);
    gameState.setBit(Demonstration);

    level = 0;
    emit setLevel(level);

    board->init(Demo, mapName.at(0));
    pix->setLevel(level);

    initPacman();
    initFruit();
    initMonsters();
    initEnergizers();

    gameState.clearBit(Init);

    repaint();

    timerCount = 0;
    QTimer::singleShot(playerDurMS, this, SLOT(start()));
}

void Referee::play()
{
    stop();
    stopEnergizer();

    gameState.fill(FALSE);
    gameState.setBit(Init);
    gameState.setBit(Playing);
    gameState.setBit(Player);
    gameState.setBit(Ready);

    lifes = 3;
    level = 1;
    points = 0;

    extraLifeScoreIndex = 0;
    nextExtraLifeScore = extraLifeScore[extraLifeScoreIndex];
    if (nextExtraLifeScore < 0)
        nextExtraLifeScore *= -1;

    board->init(Level, mapName.at(level));
    pix->setLevel(level);

    initPacman();
    initFruit();
    initMonsters();
    initEnergizers();

    repaint();
    emit toggleNew();
    emit setLifes(lifes);
    emit setLevel(level);
    emit setPoints(points);

    repaint(pix->rect(board->position(monsterhome, 0), tr("PLAYER ONE")), FALSE);
    repaint(pix->rect(board->position(fruithome), tr("READY!")), FALSE);

    timerCount = 0;
    QTimer::singleShot(playerDurMS, this, SLOT(ready()));
}

void Referee::ready()
{
    if (gameState.testBit(Paused) || gameState.testBit(HallOfFame)) {
        QTimer::singleShot(afterPauseMS, this, SLOT(ready()));
        return;
    }

    if (gameState.testBit(Player)) {
        emit setLifes(--lifes);
        gameState.clearBit(Player);
        gameState.clearBit(Init);
        repaint(pix->rect(board->position(monsterhome, 0), tr("PLAYER ONE")), FALSE);
        repaintFigures();
        QTimer::singleShot(playerDurMS, this, SLOT(ready()));
        return;
    }

    if (gameState.testBit(Ready)) {
        gameState.clearBit(Ready);
        repaint(pix->rect(board->position(fruithome), tr("READY!")), FALSE);
        start();
    } else {
        gameState.setBit(Ready);
        gameState.clearBit(Init);
        repaint(pix->rect(board->position(fruithome), tr("READY!")), FALSE);
        QTimer::singleShot(readyDurMS, this, SLOT(ready()));
    }
}


void Referee::levelUp()
{
    stop();
    stopEnergizer();

    gameState.setBit(LevelDone);
    pacman->setPosition(pacman->position());    // set mouthPosition to "0"
    repaint(pix->rect(pacman->position(), PacmanPix));

    timerCount = 0;
    QTimer::singleShot(levelUpPreAnimationMS, this, SLOT(levelUpPlay()));
}

void Referee::levelUpPlay()
{
    if (gameState.testBit(Ready))
        return;

    if (gameState.testBit(Paused) || gameState.testBit(HallOfFame)) {
        QTimer::singleShot(afterPauseMS, this, SLOT(levelUpPlay()));
        return;
    }

    switch (timerCount) {
        case 0 : gameState.setBit(Init);
                 setOnEnergizers();
                 repaintFigures();
                 break;
        case 1 : gameState.clearBit(LevelDone);
                 repaint(pix->rect(pacman->position(), PacmanPix));
                 break;
    }

    if (timerCount++ < 2) {
        QTimer::singleShot(levelUpAnimationMS, this, SLOT(levelUpPlay()));
        return;
    }

    gameState.clearBit(Init);

    if (gameState.testBit(Demonstration)) {
        hallOfFame();
        return;
    }

    if (level < maxLevel)
        level++;

    board->init(Level, mapName.at(level));
    pix->setLevel(level);

    initPacman();
    initFruit();
    initMonsters();
    initEnergizers();

    repaint();
    emit setLevel(level);

    ready();
}

void Referee::start()
{
    if (gameState.testBit(Ready))
        return;

    if (gameState.testBit(Paused) || gameState.testBit(HallOfFame)) {
        QTimer::singleShot(afterPauseMS, this, SLOT(start()));
        return;
    }

    if (gameState.testBit(Scoring)) {
        gameState.clearBit(Scoring);
        repaint(pix->rect(pix->rect(pacman->position(), MonsterPix),
                          pix->rect(pacman->position(), MonsterScorePix, monstersEaten-1)));
    }
        
    if (!gameTimer)
        gameTimer = startTimer( speed [level] );

    if (!energizerTimer)
        energizerTimer = startTimer( energizerAnimationMS );
}

void Referee::start(int t)
{
    gameTimer = startTimer(t);
}

void Referee::stop()
{
    if (gameTimer) {
        killTimer (gameTimer);
        gameTimer = 0;
    }
}

void Referee::stopEnergizer()
{
    if (energizerTimer) {
        killTimer (energizerTimer);
        energizerTimer = 0;
    }
}

void Referee::killed()
{
    if (gameState.testBit(Ready))
        return;

    if (!gameState.testBit(Dying)) {
        gameState.setBit(Dying);

        pacman->setDirection(X, TRUE);
        for (Monster *m = monsters->first(); m != 0; m = monsters->next())
            if (m)
                m->setDirection(X);
        QTimer::singleShot(dyingPreAnimationMS, this, SLOT(killed()));
    } else {
        stop();
        if (gameState.testBit(Paused) || gameState.testBit(HallOfFame)) {
            QTimer::singleShot(afterPauseMS, this, SLOT(killed()));
            return;
        }

        gameState.setBit(Init);

        repaintFigures();

        timerCount = 0;
        killedPlay();
    }
}

void Referee::killedPlay()
{
    if (!gameState.testBit(Dying) || gameState.testBit(Ready))
        return;
    if (gameState.testBit(Paused) || gameState.testBit(HallOfFame)) {
        QTimer::singleShot(afterPauseMS, this, SLOT(killedPlay()));
        return;
    }

    if (timerCount <= pix->maxPixmaps(DyingPix)) {
        repaint(pix->rect(pacman->position(), PacmanPix), FALSE);
        if (timerCount >= pix->maxPixmaps(DyingPix)-1 || timerCount == 0)
            QTimer::singleShot(dyingPostAnimationMS, this, SLOT(killedPlay()));
        else
            QTimer::singleShot(dyingAnimationMS, this, SLOT(killedPlay()));
        timerCount++;
    } else {
        gameState.clearBit(Dying);
        stopEnergizer();
        if (lifes == 0) {
            gameState.setBit(GameOver);
            gameState.clearBit(Playing);
            for (int e = 0; e < board->energizers(); e++) {
                energizers->at(e)->setOff();
                repaint(pix->rect(board->position(energizer, e), EnergizerPix), FALSE);
            }
            repaint(pix->rect(board->position(fruithome), tr("GAME  OVER")), FALSE);
            QTimer::singleShot(gameOverDurMS, this, SLOT(hallOfFame()));
        } else {
            gameState.clearBit(Init);
            initPacman();
            initFruit();
            initMonsters();
            initEnergizers();
            emit setLifes(--lifes);
            repaintFigures();
            ready();
        }
    }
}

void Referee::initPacman()
{
    pacman->setMaxPixmaps(pix->maxPixmaps(PacmanPix));
    pacman->setDemo(gameState.testBit(Demonstration));
    pacman->setPosition(board->position(pacmanhome));
    pacman->setDirection(W, TRUE);
    pacman->setAlive(pacmanTicks[level]);
}

void Referee::initFruit(bool fullInitialization)
{
    if (fullInitialization) {
        fruit->setMaxPixmaps(pix->maxPixmaps(FruitPix));
        if (fruitIndex[level] == 0)
            fruit->setLevel(level, fruitAppearsTicks[level],
                            fruitDurTicks[level], fruitTicks[level]);
        else if (fruitIndex[level] < 0)
            fruit->setLevel(pix->maxPixmaps(FruitPix)+1,
                            fruitAppearsTicks[level],
                            fruitDurTicks[level], fruitTicks[level]);
        else
            fruit->setLevel(fruitIndex[level], fruitAppearsTicks[level],
                            fruitDurTicks[level], fruitTicks[level]);
    }

    if (board->tunnels() > 0)
        fruit->setMovement(board->position(tunnel, rand() % board->tunnels()),
                           board->position(tunnel, rand() % board->tunnels()),
                           fruitIQ[level]);
}

void Referee::initMonsters()
{
    if( !monsters->isEmpty())
        monsters->clear();
    if( !monsterRect->isEmpty())
        monsterRect->clear();

    for (int id = 0; id < (gameState.testBit(Introducing) ? 4 : board->monsters()); id++) {
        Monster *m = new Monster(board, id);
        monsters->append(m);
        QRect *r = new QRect();
        monsterRect->append(r);
        if (!gameState.testBit(Introducing)) {
            m->setFreedom(board->position(prisonexit));
            m->setDangerous(dangerousTicks[level], monsterIQ[level]);
            if (id == 0)
                m->setPrison(board->position(prisonentry));
            else {
                m->setPrison(board->position(monsterhome, id));
                m->setArrested(arrestTicks[level], arrestDurTicks[level]*id);
            }
            m->setPosition(board->position(monsterhome, id));
            switch(id) {
                case 0  : m->setDirection(W); break;
                case 1  : m->setDirection(N); break;
                default : m->setDirection(S);
            }
        }
        m->setMaxPixmaps(pix->maxPixmaps(MonsterPix), pix->maxPixmaps(EyesPix));
    }
}

void Referee::initEnergizers()
{
    if( !energizers->isEmpty())
        energizers->clear();
    if( !energizerRect->isEmpty())
        energizerRect->clear();

    for (int id = 0; id < (gameState.testBit(Introducing) ? 2 : board->energizers()); id++) {
        Energizer *e = new Energizer(board);
        energizers->append(e);
        QRect *r = new QRect();
        energizerRect->append(r);
        if (!gameState.testBit(Introducing)) {
            e->setPosition(board->position(energizer, id));
            e->setOn();
        }
        e->setMaxPixmaps(pix->maxPixmaps(EnergizerPix));
    }
}

void Referee::setOnEnergizers()
{
    for (int e = 0; e < board->energizers(); e++) {
        energizers->at(e)->setOn();
    }
}

void Referee::setFocusOutPause(bool FocusOutPause)
{
    focusOutPause = FocusOutPause;
}

void Referee::setFocusInContinue(bool FocusInContinue)
{
    focusInContinue = FocusInContinue;
}

void Referee::focusInEvent(QFocusEvent *)
{
    if (focusInContinue && focusedPause &&
        gameState.testBit(Paused) && gameState.testBit(Playing)) {
        pause();
    }
}

void Referee::focusOutEvent(QFocusEvent *)
{
    if (focusOutPause && !gameState.testBit(Paused) && gameState.testBit(Playing)) {
        focusedPause = TRUE;
        pause();
    }
}
