#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef QWS
#include <qpe/qpeapplication.h>
#include <qpe/config.h>
#else
#include <qapplication.h>
#endif

#include "sfcave.h"

#define CAPTION "SFCave 1.7 by AndyQ"

// States
#define STATE_BOSS              0
#define STATE_RUNNING           1
#define STATE_CRASHED           2
#define STATE_NEWGAME           3
#define STATE_MENU              4

// Menus
#define MENU_MAIN_MENU          0
#define MENU_OPTIONS_MENU       1

// Main Menu Options
#define MENU_START_GAME         0
#define MENU_OPTIONS            1
#define MENU_QUIT               2

// Option Menu Options
#define MENU_GAME_TYPE          0
#define MENU_GAME_DIFFICULTY     1
#define MENU_BACK               2

QString SFCave::menuOptions[2][5] = { { "Start Game", "Options", "Quit", "", "" },
                                    { "Game Type - %s", "Game Difficulty - %s", "Back", "", "" } };


#define NR_GAME_DIFFICULTIES    3
#define NR_GAME_TYPES           3

#define EASY                    "Easy"
#define NORMAL                  "Normal"
#define HARD                    "Hard"

#define SFCAVE_GAME_TYPE        0
#define GATES_GAME_TYPE         1
#define FLY_GAME_TYPE           2
#define SFCAVE_GAME             "SFCave"
#define GATES_GAME              "Gates"
#define FLY_GAME                "Fly"
QString SFCave::dificultyOption[] = { EASY, NORMAL, HARD };
QString SFCave::gameTypes[] = { SFCAVE_GAME, GATES_GAME, FLY_GAME };

#define CURRENT_GAME_TYPE       gameTypes[currentGameType]
#define CURRENT_GAME_DIFFICULTY difficultyOption[currentGameDifficulty];

bool movel;


int main( int argc, char *argv[] )
{
    movel = true;

#ifdef QWS
    QPEApplication a( argc, argv );
#else
    QApplication a( argc, argv );
#endif

    int speed = 3;
    for ( int i = 0 ; i < argc ; ++i )
    {
        if ( strcmp( argv[i], "-s" ) == 0 )
        {
            if ( i+1 < argc )
                speed = atoi( argv[i+1] );
        }
    }

    SFCave app( speed );
    a.setMainWidget( &app );
    app.show();
    app.start();
    a.exec();
}

SFCave :: SFCave( int spd, QWidget *w, char *name )
    : QMainWindow( w, name )

{
#ifdef QWS
    showMaximized();
#else
    resize( 240, 284 );
#endif

    sWidth = width();
    sHeight = height();
    segSize = sWidth/(MAPSIZE-1)+1;

    currentMenuNr = 0;
    nrMenuOptions[0] = 3;
    nrMenuOptions[1] = 3;
    currentMenuOption[0] = 0;
    currentMenuOption[1] = 0;
    currentGameType = 0;
    currentGameDifficulty = 0;

    setCaption( CAPTION );

#ifdef QWS
    Config cfg( "sfcave" );
    cfg.setGroup( "settings" );
    QString key = "highScore_";
    highestScore[SFCAVE_GAME_TYPE] = cfg.readNumEntry( key + SFCAVE_GAME, 0 );
    highestScore[GATES_GAME_TYPE] = cfg.readNumEntry( key + GATES_GAME, 0 );
    highestScore[FLY_GAME_TYPE] = cfg.readNumEntry( key + FLY_GAME, 0 );
    currentGameType = cfg.readNumEntry( "gameType", 0 );
    currentGameDifficulty = cfg.readNumEntry( "difficulty", 0 );
#endif
    speed = spd; // Change to 2 for PC
    press = false;

    offscreen = new QPixmap( sWidth, sHeight );
    offscreen->fill( Qt::black );

    setUp();
    crashLineLength = -1;

    gameTimer = new QTimer( this, "game timer" );
    connect( gameTimer, SIGNAL( timeout() ),
             this, SLOT( run() ) );
}

SFCave :: ~SFCave()
{
}

void SFCave :: start()
{
    gameTimer->start( 10 );

}

int SFCave :: nextInt( int range )
{
    return rand() % range;
}

void SFCave :: setUp()
{
    state = STATE_MENU;
    prevState = STATE_MENU;

    score = 0;
    offset = 0;
    maxHeight = 50;
    nrFrames = 0;
    dir = 1;
    thrust = 0;
    crashLineLength = 0;

    user.setRect( 50, sWidth/2, 4, 4 );

    blockWidth = 20;
    blockHeight = 70;
    gapHeight = 125;
    gateDistance = 75;
    nextGate = nextInt( 50 ) + gateDistance;

    for ( int i = 0 ; i < TRAILSIZE ; ++i )
    {
        trail[i].setX( -1 );
        trail[i].setY( 0 );
    }

    mapTop[0] = (int)(nextInt(50)) + 5;
    for ( int i = 1 ; i < MAPSIZE ; ++i )
        setPoint( i );

    for ( int i = 0 ; i < BLOCKSIZE ; ++i )
        blocks[i].setY( -1 );
}

void SFCave :: run()
{
    //running = true;
    //setUp();
        switch ( state )
        {
            case STATE_MENU:
                displayMenu();
                break;
            case STATE_NEWGAME:
                setUp();
                draw();
                state = STATE_RUNNING;
                break;
            case STATE_BOSS:
                drawBoss();
                break;

            case STATE_CRASHED:
                draw();
                break;

            case STATE_RUNNING:
            {
                if ( nrFrames % 5 == 0 )
                    score ++;
                if ( nrFrames % 2 == 0 )
                    handleKeys();

                // Apply Game rules
                nrFrames ++;
                if ( CURRENT_GAME_TYPE == SFCAVE_GAME )
                {
                    if ( nrFrames % 500 == 0 )
                    {
                        if ( maxHeight < sHeight - 100 )
                        {
                            maxHeight += 10;

                            // Reduce block height
                            if ( maxHeight > sHeight - 150 )
                                blockHeight -= 5;
                        }
                    }

                    if ( nrFrames % 100 == 0 )
                        addBlock();
                }
                else if ( CURRENT_GAME_TYPE == GATES_GAME )
                {
                    // Slightly random gap distance
                    if ( nrFrames >= nextGate )
                    {
                        nextGate = nrFrames + nextInt( 50 ) + gateDistance;
                        addGate();
                    }

                    if ( nrFrames % 500 == 0 )
                    {
                        if ( gapHeight > 75 )
                            gapHeight -= 5;
                    }
                }
                else if ( CURRENT_GAME_TYPE == FLY_GAME )
                {
                }
                    
                
                if ( checkCollision() )
                {
                    if ( score > highestScore[currentGameType] )
                        highestScore[currentGameType] = score;

#ifdef QWS
    Config cfg( "sfcave" );
    cfg.setGroup( "settings" );
    QString key = "highScore_";
    key += CURRENT_GAME_TYPE;
    cfg.writeEntry( key, highestScore[currentGameType] );
#endif

                    state = STATE_CRASHED;
                }
                else
                {
                    if ( movel )
                        moveLandscape();
                    //movel = false;
                }

                draw();
                break;
            }
        }
}

bool SFCave :: checkCollision()
{
    if ( (user.y() + user.width()) >= mapBottom[10] || user.y() <= mapTop[10] )
        return true;

    for ( int i = 0 ; i < BLOCKSIZE ; ++i )
    {
        if ( blocks[i].y() != -1 )
        {
            if ( blocks[i].intersects( user ) )
                return true;
        }
    }
    return false;
}

void SFCave :: moveLandscape()
{
    offset++;

    if ( offset >= segSize )
    {
        offset = 0;
        for ( int i = 0 ; i < MAPSIZE-speed ; ++i )
        {
            mapTop[i] = mapTop[i+speed];
            mapBottom[i] = mapBottom[i+speed];
        }

        for ( int i = speed ; i > 0 ; --i )
            setPoint( MAPSIZE-i );
    }

    for ( int i = 0 ; i < BLOCKSIZE ; ++i )
    {
        if ( blocks[i].y() != -1 )
        {
            blocks[i].moveBy( -speed, 0 );
            if ( blocks[i].x() + blocks[i].width() < 0 )
                blocks[i].setY( -1 );
        }
    }
}

void SFCave :: addBlock()
{
    for ( int i = 0 ; i < BLOCKSIZE ; ++i )
    {
        if ( blocks[i].y() == -1 )
        {
            int x = sWidth;

            int y = mapTop[50] + (int)(nextInt(mapBottom[50] - mapTop[50] - blockHeight));

            blocks[i].setRect( x, y, blockWidth, blockHeight );

            break;
        }
    }
}

void SFCave :: addGate()
{
    for ( int i = 0 ; i < BLOCKSIZE ; ++i )
    {
        if ( blocks[i].y() == -1 )
        {
            int x1 = sWidth;
            int y1 = mapTop[50];
            int b1Height = nextInt(mapBottom[50] - mapTop[50] - gateDistance);

            // See if height between last gate and this one is too big
            if ( b1Height - 200 > lastGateBottomY )
                b1Height -= 25;
            else if ( b1Height + 200 < lastGateBottomY )
                b1Height += 25;
            lastGateBottomY = b1Height;

            int x2 = sWidth;
            int y2 = y1 + b1Height + gateDistance;
            int b2Height = mapBottom[50] - y2;
            

            blocks[i].setRect( x1, y1, blockWidth, b1Height );
            blocks[i+1].setRect( x2, y2, blockWidth, b2Height );

            break;
        }
    }
}

void SFCave :: setPoint( int point )
{
    if ( nextInt(100) >= 80 )
        dir *= -1;

    mapTop[point] = mapTop[point-1] + (dir * nextInt( 5 ) );
    if ( mapTop[point] < 0 )
    {
        mapTop[point] = 0;
        dir *= -1;
    }
    else if ( mapTop[point] >= maxHeight )
    {
        mapTop[point] = maxHeight;
        dir *= -1;
    }

    mapBottom[point] = sHeight - (maxHeight - mapBottom[point]);
    mapBottom[point] = sHeight - (maxHeight - mapTop[point]);
}

void SFCave :: drawBoss()
{
    offscreen->fill( Qt::black );

    bitBlt( this, 0, 0, offscreen, 0, 0, sWidth, sHeight, Qt::CopyROP, true );
}

void SFCave :: draw()
{
    //printf( "Paint\n" );
    offscreen->fill( Qt::black );

    QPainter p( offscreen );
    QFontMetrics fm = p.fontMetrics();
    p.setPen( Qt::white );

    for ( int i = 0 ; i < MAPSIZE -3; ++i )
    {
        p.drawLine( (i*segSize) - (offset*speed), mapTop[i], ((i+1)*segSize)-(offset*speed), mapTop[i+1] );
        p.drawLine( (i*segSize) - (offset*speed), mapBottom[i], ((i+1)*segSize)-(offset*speed), mapBottom[i+1] );
    }

    // Draw user
    p.drawRect( user );

    // Draw trails
    for ( int i = 0 ; i < TRAILSIZE ; ++i )
        if ( trail[i].x() >= 0 )
            p.drawRect( trail[i].x(), trail[i].y(), 2, 2 );

    // Draw blocks
    for ( int i = 0 ; i < BLOCKSIZE ; ++i )
        if ( blocks[i].y() != -1 )
        {
            p.fillRect( blocks[i], Qt::black );
            p.drawRect( blocks[i] );
        }

    // draw score
    QString s;
    s.sprintf( "score %06d   high score %06d", score, highestScore[currentGameType] );
    p.drawText( 5, 10, s );


    if ( state == STATE_CRASHED )
    {
        // add next crash line

        if ( crashLineLength != -1 )
        {
            for ( int i = 0 ; i < 36 ; ++i )
            {
                int x = (int)(user.x() + (crashLineLength+nextInt(10)) * cos( (M_PI/180) * (10.0 * i) ) );
                int y = (int)(user.y() + (crashLineLength+nextInt(10)) * sin( (M_PI/180) * (10.0 * i) ) );            p.drawLine( user.x(), user.y(), x, y );
            }
        }

        if ( crashLineLength >= 15 || crashLineLength == -1 )
        {
            QString text = "Press up or down to start";
            p.drawText( (sWidth/2) - (fm.width( text )/2), 140, text );

            text = "or press OK for menu";
            p.drawText( (sWidth/2) - (fm.width( text )/2), 155, text );
//           p.drawText( 70, 140, QString( "Press down to start" ) );
        }
        else
            crashLineLength ++;
    }

    p.end();
    bitBlt( this, 0, 0, offscreen, 0, 0, sWidth, sHeight, Qt::CopyROP, true );
    //printf( "endpaint\n" );
}

void SFCave :: handleKeys()
{
    // Find enpty trail and move others
    bool done = false;
    for ( int i = 0 ; i < TRAILSIZE ; ++i )
    {
        if ( trail[i].x() < 0 )
        {
            if ( !done )
            {
                trail[i].setX( user.x() - 5 );
                trail[i].setY( user.y() );
                done = true;
            }
        }
        else
        {
            trail[i].setX( trail[i].x() - (2) );
        }
    }

    if ( speed <= 3 )
    {
        if ( press )
            thrust -= 0.6;
        else
            thrust += 0.8;

        if ( thrust > 4.0 )
            thrust = 4.0;
        else if ( thrust < -3.5 )
            thrust = -3.5;
    }
    else
    {
        if ( press )
            thrust -= 0.5;
        else
            thrust += 0.8;

        if ( thrust > 5.0 )
            thrust = 5.0;
        else if ( thrust < -3.5 )
            thrust = -3.5;
    }
    user.moveBy( 0, (int)thrust );
}

void SFCave :: keyPressEvent( QKeyEvent *e )
{
    if ( state == STATE_MENU )
    {
        switch( e->key() )
        {
            case Qt::Key_Down:
                currentMenuOption[currentMenuNr] ++;
                if ( menuOptions[currentMenuNr][currentMenuOption[currentMenuNr]] == "" )
                    currentMenuOption[currentMenuNr] = 0;
                break;
            case Qt::Key_Up:
                currentMenuOption[currentMenuNr] --;
                if ( currentMenuOption[currentMenuNr] < 0 )
                    currentMenuOption[currentMenuNr] = nrMenuOptions[currentMenuNr]-1;
                break;

            case Qt::Key_Left:
                if ( currentMenuNr == MENU_OPTIONS_MENU )
                {
                    if ( currentMenuOption[currentMenuNr] == MENU_GAME_TYPE )
                    {
                        currentGameType --;
                        if ( currentGameType < 0 )
                            currentGameType = NR_GAME_TYPES - 1;
                    }
                    else if ( currentMenuOption[currentMenuNr] == MENU_GAME_DIFFICULTY )
                    {
                        currentGameDifficulty --;
                        if ( currentGameDifficulty < 0 )
                            currentGameDifficulty = NR_GAME_DIFFICULTIES - 1;
                    }
                }
                break;

            case Qt::Key_Right:
                if ( currentMenuNr == MENU_OPTIONS_MENU )
                {
                    if ( currentMenuOption[currentMenuNr] == MENU_GAME_TYPE )
                    {
                        currentGameType ++;
                        if ( currentGameType == NR_GAME_TYPES )
                            currentGameType = 0;
                    }
                    else if ( currentMenuOption[currentMenuNr] == MENU_GAME_DIFFICULTY )
                    {
                        currentGameDifficulty ++;
                        if ( currentGameDifficulty == NR_GAME_DIFFICULTIES )
                            currentGameDifficulty = 0;
                    }
                }
                break;

            case Qt::Key_Space:
            case Qt::Key_Return:
            case Qt::Key_Enter:
                dealWithMenuSelection();
                break;
        }
    }
    else
    {
        switch( e->key() )
        {
            case Qt::Key_Up:
            case Qt::Key_F9:
            case Qt::Key_Space:
                press = true;
                break;
            case Qt::Key_M:
            case Qt::Key_Return:
            case Qt::Key_Enter:
                if ( state == STATE_CRASHED && crashLineLength >= 15 || crashLineLength == -1 )
                    state = STATE_MENU;
           default:
                e->ignore();
                break;
        }
    }
}

void SFCave :: keyReleaseEvent( QKeyEvent *e )
{
    if ( state == STATE_MENU )
    {
    }
    else
    {
        switch( e->key() )
        {
            case Qt::Key_F9:
            case Qt::Key_Space:
                press = false;
                break;

            case Qt::Key_Up:
                press = false;
                
            case Qt::Key_R:
            case Qt::Key_Down:
                if ( state == STATE_CRASHED && crashLineLength >= 15 || crashLineLength == -1 )
                {
                    state = STATE_NEWGAME;
                }
                else
                    movel = true;
                break;

           default:
                e->ignore();
                break;
        }
    }

}

void SFCave :: displayMenu()
{
    offscreen->fill( Qt::black );

    QPainter p( offscreen );
    p.setPen( Qt::white );

    QFont f( "Helvetica", 16 );
    p.setFont( f );

    QFontMetrics fm = p.fontMetrics();

    QString text = "SFCave";
    p.drawText( (sWidth/2) - (fm.width( text )/2), 60, text );

    text = "Written by Andy Qua";
    p.drawText( (sWidth/2) - (fm.width( text )/2), 85, text );

    // Draw options
    int pos = 170;
    for ( int i = 0 ; menuOptions[currentMenuNr][i] != "" ; ++i, pos += 25 )
    {
        if ( currentMenuOption[currentMenuNr] == i )
            p.setPen( Qt::yellow );
        else
            p.setPen( Qt::white );

        QString text;
        if ( menuOptions[currentMenuNr][i].find( "%s" ) != -1 )
        {
            QString val;
            if ( i == MENU_GAME_TYPE )
                val = gameTypes[currentGameType];
            else
                val = dificultyOption[currentGameDifficulty];

            text.sprintf( (const char *)menuOptions[currentMenuNr][i], (const char *)val );
        }
        else
            text = menuOptions[currentMenuNr][i];

        p.drawText( (sWidth/2) - (fm.width( text )/2), pos, text );
    }

    p.end();
    bitBlt( this, 0, 0, offscreen, 0, 0, sWidth, sHeight, Qt::CopyROP, true );
}

void SFCave :: dealWithMenuSelection()
{
    switch( currentMenuNr )
    {
        case MENU_MAIN_MENU:
        {
            switch( currentMenuOption[currentMenuNr] )
            {
                case MENU_START_GAME:
                    state = STATE_NEWGAME;
                    break;

                case MENU_OPTIONS:
                    currentMenuNr = MENU_OPTIONS_MENU;
                    currentMenuOption[currentMenuNr] = 0;
                    break;

                case MENU_QUIT:
                    QApplication::exit();
                    break;
            }

            break;
        }

        case MENU_OPTIONS_MENU:
        {
            switch( currentMenuOption[currentMenuNr] )
            {
                case MENU_GAME_TYPE:
                    break;
                
                case MENU_GAME_DIFFICULTY:
                    break;
                    
               case MENU_BACK:
                    currentMenuNr = MENU_MAIN_MENU;

#ifdef QWS
                    Config cfg( "sfcave" );
                    cfg.setGroup( "settings" );
                    cfg.writeEntry( "difficulty", currentGameDifficulty );
                    cfg.writeEntry( "gameType", currentGameType );
#endif
                    break;
            }

            break;
        }
    }      
}