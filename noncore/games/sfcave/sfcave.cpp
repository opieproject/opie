#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#ifdef QWS
#include <qpe/qpeapplication.h>
#include <qpe/config.h>
#else
#include <qapplication.h>
#endif
#include <qdir.h>

#include "helpwindow.h"
#include "sfcave.h"

#define CAPTION "SFCave 1.10 by AndyQ"

#define UP_THRUST               0.6
#define NO_THRUST               0.8
#define MAX_DOWN_THRUST         4.0
#define MAX_UP_THRUST           -3.5

// States
#define STATE_BOSS              0
#define STATE_RUNNING           1
#define STATE_CRASHING          2
#define STATE_CRASHED           3
#define STATE_NEWGAME           4
#define STATE_MENU              5
#define STATE_REPLAY            6

// Menus
#define MENU_MAIN_MENU          0
#define MENU_OPTIONS_MENU       1

// Main Menu Options
#define MENU_START_GAME         0
#define MENU_OPTIONS            1
#define MENU_HELP               2
#define MENU_QUIT               3

// Option Menu Options
#define MENU_GAME_TYPE          0
#define MENU_GAME_DIFFICULTY    1
#define MENU_CLEAR_HIGHSCORES   2
#define MENU_BACK               3


#define NR_GAME_DIFFICULTIES    3
#define NR_GAME_TYPES           3

#define DIFICULTY_EASY          0
#define DIFICULTY_NORMAL        1
#define DIFICULTY_HARD          2
#define EASY                    "Easy"
#define NORMAL                  "Normal"
#define HARD                    "Hard"

#define SFCAVE_GAME_TYPE        0
#define GATES_GAME_TYPE         1
#define FLY_GAME_TYPE           2
#define SFCAVE_GAME             "SFCave"
#define GATES_GAME              "Gates"
#define FLY_GAME                "Fly"
#define CURRENT_GAME_TYPE       gameTypes[currentGameType]
#define CURRENT_GAME_DIFFICULTY difficultyOption[currentGameDifficulty];

QString SFCave::dificultyOption[] = { EASY, NORMAL, HARD };
QString SFCave::gameTypes[] = { SFCAVE_GAME, GATES_GAME, FLY_GAME };

QString SFCave::menuOptions[2][5] = { { "Start Game", "Options", "Help", "Quit", "" },
                                    { "Game Type - %s", "Game Difficulty - %s", "Clear High Scores for this game", "Back", "" } };

#define UP_THRUST               0.6
#define NO_THRUST               0.8
#define MAX_DOWN_THRUST         4.0
#define MAX_UP_THRUST           -3.5
double SFCave::UpThrustVals[3][3]               = {{ 0.6,   0.6, 0.6 },     // SFCave
                                                   { 0.6,   0.6, 0.8 },     // Gates
                                                   { 0.4,   0.7, 1.0 } };   // Fly
                                                   
double SFCave::DownThrustVals[3][3]             = {{ 0.8,   0.8, 0.8 },     // SFCave
                                                   { 0.8,   0.8, 1.0 },     // Gates
                                                   { 0.4,   0.7, 1.0 } };   // Fly
                                                   
double SFCave::MaxUpThrustVals[3][3]            = {{ -3.5, -3.5, -3.5 },    // SFCave
                                                   { -3.5, -4.0, -5.0 },    // Gates
                                                   { -3.5, -4.0, -5.0 } };  // Fly
                                                   
double SFCave::MaxDownThrustVals[3][3]          = {{ 4.0,   4.0, 4.0 },     // SFCave
                                                   { 4.0,   5.0, 5.5 },     // Gates
                                                   { 3.5,   4.0, 5.0 } };   // Fly

int SFCave::initialGateGaps[]           = { 75, 50, 25 };

int SFCave::nrMenuOptions[2] = { 4, 4 };
int SFCave ::currentMenuOption[2] = { 0, 0 };

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
    replayIt = 0;
#ifdef QWS
    showMaximized();
#else
    resize( 240, 284 );
#endif

    replayFile = QDir::home().path();
    replayFile += "/sfcave.replay";
    printf( "%s\n", (const char *)replayFile );

    sWidth = width();
    sHeight = height();
    segSize = sWidth/(MAPSIZE-1)+1;

    currentMenuNr = 0;
    currentGameType = 0;
    currentGameDifficulty = 0;

    setCaption( CAPTION );
    showScoreZones = false;

#ifdef QWS
    Config cfg( "sfcave" );
    cfg.setGroup( "settings" );
    QString key = "highScore_";

    for ( int i = 0 ; i < 3 ; ++i )
    {
        for ( int j = 0 ; j < 3 ; ++j )
            highestScore[i][j] = cfg.readNumEntry( key + gameTypes[i] + "_" + dificultyOption[j], 0 );
    }
    
    currentGameType = cfg.readNumEntry( "gameType", 0 );
    currentGameDifficulty = cfg.readNumEntry( "difficulty", 0 );
#endif
    speed = spd; // Change to 2 for PC
    press = false;

    offscreen = new QPixmap( sWidth, sHeight );
    offscreen->fill( Qt::black );

//    setUp();
    crashLineLength = -1;
    state = STATE_MENU;
    prevState = STATE_MENU;

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

void SFCave :: setSeed( int seed )
{
    if ( seed == -1 )
        currentSeed = ((unsigned long) time((time_t *) NULL));
    else
        currentSeed = seed;
    PutSeed( currentSeed );
}
    
int SFCave :: nextInt( int range )
{
    int val = (int)(Random( ) * range);

    return val;
    
}

void SFCave :: setUp()
{
    score = 0;
    offset = 0;
    nrFrames = 0;
    dir = 1;
    thrust = 0;

    if ( CURRENT_GAME_TYPE == SFCAVE_GAME )
    {
        thrustUp = UpThrustVals[SFCAVE_GAME_TYPE][currentGameDifficulty];;
        noThrust = DownThrustVals[SFCAVE_GAME_TYPE][currentGameDifficulty];;
        maxUpThrust = MaxUpThrustVals[SFCAVE_GAME_TYPE][currentGameDifficulty];;
        maxDownThrust = MaxDownThrustVals[SFCAVE_GAME_TYPE][currentGameDifficulty];;
    }
    else if ( CURRENT_GAME_TYPE == GATES_GAME )
    {
        thrustUp = UpThrustVals[GATES_GAME_TYPE][currentGameDifficulty];;
        noThrust = DownThrustVals[GATES_GAME_TYPE][currentGameDifficulty];;
        maxUpThrust = MaxUpThrustVals[GATES_GAME_TYPE][currentGameDifficulty];;
        maxDownThrust = MaxDownThrustVals[GATES_GAME_TYPE][currentGameDifficulty];;
    }
    else
    {
        thrustUp = UpThrustVals[FLY_GAME_TYPE][currentGameDifficulty];
        noThrust = DownThrustVals[FLY_GAME_TYPE][currentGameDifficulty];
        maxUpThrust = MaxUpThrustVals[FLY_GAME_TYPE][currentGameDifficulty];
        maxDownThrust = MaxDownThrustVals[FLY_GAME_TYPE][currentGameDifficulty];
    }
    
    crashLineLength = 0;
    lastGateBottomY = 0;

    user.setRect( 50, sWidth/2, 4, 4 );

    blockWidth = 20;
    blockHeight = 70;
    gapHeight = initialGateGaps[currentGameDifficulty];
    gateDistance = 75;
    nextGate = nextInt( 50 ) + gateDistance;

    for ( int i = 0 ; i < TRAILSIZE ; ++i )
    {
        trail[i].setX( -1 );
        trail[i].setY( 0 );
    }

    if ( CURRENT_GAME_TYPE != FLY_GAME )
    {
        maxHeight = 50;
        
        mapTop[0] = (int)(nextInt(50)) + 5;
        mapBottom[0] = (int)(nextInt(50)) + 5;
        for ( int i = 1 ; i < MAPSIZE ; ++i )
            setPoint( i );
    }
    else
    {
        maxHeight = 100;

        for ( int i = 0 ; i < MAPSIZE ; ++i )
            mapBottom[i] = sHeight - 10;
    }
    for ( int i = 0 ; i < BLOCKSIZE ; ++i )
        blocks[i].setY( -1 );

}

void SFCave :: run()
{
    switch ( state )
    {
        case STATE_MENU:
            displayMenu();
            break;
        case STATE_NEWGAME:
            setSeed( -1 );
            setUp();
            draw();
            state = STATE_RUNNING;
            replay = false;
            replayList.clear();
            break;
        case STATE_REPLAY:
            setSeed( currentSeed );
            setUp();
            draw();
            state = STATE_RUNNING;
            replay = true;
            if ( replayIt )
                delete replayIt;
            replayIt = new QListIterator<int>( replayList );
        case STATE_BOSS:
            drawBoss();
            break;

        case STATE_CRASHING:
        case STATE_CRASHED:
            draw();
            break;

        case STATE_RUNNING:
        {
            if ( nrFrames % 2 == 0 )
                handleKeys();

            // Apply Game rules
            nrFrames ++;

            if ( replay )
            {
                while( replayIt->current() && *(replayIt->current()) == nrFrames )
                {
                    press = !press;
                    ++(*replayIt);
                }
            }
            
            if ( CURRENT_GAME_TYPE == SFCAVE_GAME )
                handleGameSFCave();
            else if ( CURRENT_GAME_TYPE == GATES_GAME )
                handleGameGates();
            else if ( CURRENT_GAME_TYPE == FLY_GAME )
                handleGameFly();

            draw();
            break;
        }
    }
}

void SFCave :: handleGameSFCave()
{
    // Update score
    if ( nrFrames % 5 == 0 )
        score ++;

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

    if ( checkCollision() )
    {
        if ( score > highestScore[currentGameType][currentGameDifficulty] )
        {
            highestScore[currentGameType][currentGameDifficulty] = score;
            saveScore();
        }
        state = STATE_CRASHING;
    }
    else
    {
        moveLandscape();
    }

}


void SFCave :: handleGameGates()
{
    // Update score
    if ( nrFrames % 5 == 0 )
        score ++;

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

    if ( checkCollision() )
    {
        if ( score > highestScore[currentGameType][currentGameDifficulty] )
        {
            highestScore[currentGameType][currentGameDifficulty] = score;
            saveScore();
        }
        state = STATE_CRASHING;
    }
    else
    {
        moveLandscape();
    }

}

void SFCave :: handleGameFly()
{
    if ( nrFrames % 4 == 0 )
    {
        // Update score
        // get distance between landscape and ship
        int diff = mapBottom[10] - user.y();

        // the closer the difference is to 0 means more points
        if ( diff < 10 )
            score += 5;
        else if ( diff < 20 )
            score += 3;
        else if ( diff < 30 )
            score += 2;
        else if ( diff < 40 )
            score += 1;
    }

    if ( checkFlyGameCollision() )
    {
        if ( score > highestScore[currentGameType][currentGameDifficulty] )
        {
            highestScore[currentGameType][currentGameDifficulty] = score;
            saveScore();
        }
        state = STATE_CRASHING;
    }
    else
    {
        moveFlyGameLandscape();
    }
}

bool SFCave :: checkFlyGameCollision()
{
    if ( (user.y() + user.width()) >= mapBottom[11] )
        return true;

    return false;
}

void SFCave :: moveFlyGameLandscape()
{
    offset++;

    if ( offset >= segSize )
    {
        offset = 0;
        for ( int i = 0 ; i < MAPSIZE-speed ; ++i )
            mapBottom[i] = mapBottom[i+speed];

        for ( int i = speed ; i > 0 ; --i )
            setFlyPoint( MAPSIZE-i );
    }
}

void SFCave :: setFlyPoint( int point )
{
    static int fly_difficulty_levels[] = { 5, 10, 15 };
    if ( nextInt(100) >= 75 )
        dir *= -1;

    int prevPoint = mapBottom[point-1];
    
    int nextPoint = prevPoint + (dir * nextInt( fly_difficulty_levels[currentGameDifficulty] ) );

    if ( nextPoint > sHeight )
    {
        nextPoint = sHeight;
        dir *= -1;
    }
    else if ( nextPoint < maxHeight )
    {
        nextPoint = maxHeight;
        dir *= 1;
    }

    mapBottom[point] = nextPoint;
}

bool SFCave :: checkCollision()
{
    if ( (user.y() + user.width()) >= mapBottom[11] || user.y() <= mapTop[11] )
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
            int b1Height = nextInt(mapBottom[50] - mapTop[50] - gapHeight);

            // See if height between last gate and this one is too big
            if ( b1Height - 100 > lastGateBottomY )
                b1Height -= 25;
            else if ( b1Height + 100 < lastGateBottomY )
                b1Height += 25;
            lastGateBottomY = b1Height;


            int x2 = sWidth;
            int y2 = y1 + b1Height + gapHeight;
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

//    mapBottom[point] = sHeight - (maxHeight - mapBottom[point]);
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
        // Only display top landscape if not running FLY_GAME
        if ( CURRENT_GAME_TYPE != FLY_GAME )
            p.drawLine( (i*segSize) - (offset*speed), mapTop[i], ((i+1)*segSize)-(offset*speed), mapTop[i+1] );
            
        p.drawLine( (i*segSize) - (offset*speed), mapBottom[i], ((i+1)*segSize)-(offset*speed), mapBottom[i+1] );

        if ( CURRENT_GAME_TYPE == FLY_GAME && showScoreZones )
        {
            p.setPen( Qt::red );
            p.drawLine( (i*segSize) - (offset*speed), mapBottom[i]-10, ((i+1)*segSize)-(offset*speed), mapBottom[i+1]-10 );
            p.drawLine( (i*segSize) - (offset*speed), mapBottom[i]-20, ((i+1)*segSize)-(offset*speed), mapBottom[i+1]-20 );
            p.drawLine( (i*segSize) - (offset*speed), mapBottom[i]-30, ((i+1)*segSize)-(offset*speed), mapBottom[i+1]-30 );
            p.drawLine( (i*segSize) - (offset*speed), mapBottom[i]-40, ((i+1)*segSize)-(offset*speed), mapBottom[i+1]-40 );
            p.setPen( Qt::white );
        }        
    }

    // Uncomment this to show user segment (usful for checking collision boundary with landscape
//    p.setPen( Qt::red );
//    p.drawLine( (11*segSize) - (offset*speed), 0, ((11)*segSize)-(offset*speed), sHeight );
//    p.setPen( Qt::white );
    
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
    s.sprintf( "score %06d   high score %06d", score, highestScore[currentGameType][currentGameDifficulty] );
    p.drawText( 5, 10, s );


    if ( state == STATE_CRASHING || state == STATE_CRASHED )
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

        if ( state == STATE_CRASHING && crashLineLength >= 15 ) //|| crashLineLength == -1) )
            state = STATE_CRASHED;

        if ( state == STATE_CRASHED )
        {
            QString text = "Press up or down to start";
            p.drawText( (sWidth/2) - (fm.width( text )/2), 120, text );

            text = "Press OK for menu";
            p.drawText( (sWidth/2) - (fm.width( text )/2), 135, text );

            text = "Press r to replay";
            p.drawText( (sWidth/2) - (fm.width( text )/2), 150, text );
                                                           
            text = "Press s to save the replay";
            p.drawText( (sWidth/2) - (fm.width( text )/2), 165, text );

            text = "Press r to load a saved replay";
            p.drawText( (sWidth/2) - (fm.width( text )/2), 180, text );
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
            thrust -= thrustUp;
        else
            thrust += noThrust;

        if ( thrust > maxDownThrust )
            thrust = maxDownThrust;
        else if ( thrust < maxUpThrust )
            thrust = maxUpThrust;
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
                if ( !replay && !press )
                {
                    press = true;
                    replayList.append( new int( nrFrames ) );
                }
                break;
            case Qt::Key_M:
            case Qt::Key_Return:
            case Qt::Key_Enter:
                if ( state == STATE_CRASHED )
                    state = STATE_MENU;
                break;

            case Qt::Key_Z:
                showScoreZones = !showScoreZones;
                break;
                
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
            case Qt::Key_Up:
                if ( !replay && press )
                {
                    press = false;

                    replayList.append( new int( nrFrames ) );
                }
                break;

            case Qt::Key_R:
                if ( state == STATE_CRASHED )
                {
                    state = STATE_REPLAY;
                }
                break;

            case Qt::Key_Down:
                if ( state == STATE_CRASHED )
                    state = STATE_NEWGAME;
                break;

            case Qt::Key_S:
                if ( state == STATE_CRASHED )
                    saveReplay();
                break;

            case Qt::Key_L:
                if ( state == STATE_CRASHED )
                    loadReplay();
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

                case MENU_HELP:
                {
                    // Display Help Menu
                    HelpWindow *dlg = new HelpWindow( this );
                    dlg->exec();
                    delete dlg;
                    break;
                }
                
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

                case MENU_CLEAR_HIGHSCORES:
                    for ( int i = 0 ; i < 3 ; ++i )
                        highestScore[currentGameType][i] = 0;
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

void SFCave :: saveScore()
{
#ifdef QWS
    Config cfg( "sfcave" );
    cfg.setGroup( "settings" );
    QString key = "highScore_";

    cfg.writeEntry( key + gameTypes[currentGameType] + "_" + dificultyOption[currentGameDifficulty], highestScore[currentGameType][currentGameDifficulty] );
    key += CURRENT_GAME_TYPE;
    cfg.writeEntry( key, highestScore[currentGameType] );
#endif
}

void SFCave :: saveReplay()
{
    FILE *out;
    out = fopen( (const char *)replayFile, "w" );
    if ( !out )
    {
        printf( "Couldn't write to /home/root/sfcave.replay\n" );
        return;
    }

    // Build up string of values
    // Format is:: <landscape seed> <framenr> <framenr>.......
    QString val;
    val.sprintf( "%d ", currentSeed );
    
    QListIterator<int> it( replayList );
    while( it.current() )
    {
        QString tmp;
        tmp.sprintf( "%d ", (*it.current()) );
        val += tmp;

        ++it;
    }
    val += "\n";

    QString line;
    line.sprintf( "%d\n", val.length() );
    fwrite( (const char *)line, 1, line.length(), out );
    fwrite( (const char *)val, 1, val.length(), out );
   
    fclose( out );

    printf( "Replay saved to %s\n", (const char *)replayFile );

}

void SFCave :: loadReplay()
{
    FILE *in = fopen( (const char *)replayFile, "r" );

    if ( in == 0 )
    {
        printf( "Couldn't load replay file!\n" );
        return;
    }   
    // Read size of next line
    char line[10+1];
    fgets( line, 10, in );

    int length = -1;
    sscanf( line, "%d", &length );
    char *data = new char[length+1];

    fread( data, 1, length, in );

    QString sep  = " ";
    QStringList list = QStringList::split( sep, QString( data ) );
    
    // print it out
    QStringList::Iterator it = list.begin();
    currentSeed = (*it).toInt();
    ++it;

    replayList.clear();
    for ( ; it != list.end(); ++it )
    {
        int v = (*it).toInt();
        replayList.append( new int( v ) );
    }

    delete data;

    fclose( in );
    
    printf( "Replay loaded from %s\n", (const char *)replayFile );
}