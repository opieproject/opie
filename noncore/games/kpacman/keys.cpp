
#include "portable.h"

#if defined( KDE2_PORT )
#include <kapp.h>
#include <kconfig.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kaccel.h>

#include <keys.h>
#include <keys.moc>
#elif defined( QPE_PORT )
#include <qaccel.h>
#include <qpe/qpeapplication.h>
#include "config.h"
#include "keys.h"
#endif

#include <qpushbt.h>
#include <qlabel.h>
#include <qframe.h>
#include <qkeycode.h>
#include <qpixmap.h>
#include <qstring.h>

Keys::Keys( QWidget *parent, const char *name)
    : QDialog( parent, name, TRUE )
{
    //KStandardDirs *dirs = KGlobal::dirs();

    QPushButton *okButton  = new QPushButton(this);
    okButton->setText(tr("Ok"));
    okButton->setFixedSize(okButton->size());
    connect( okButton, SIGNAL(clicked()),this, SLOT(ok()) );
    okButton->move(20,210);

    QPushButton *defaultButton  = new QPushButton(this);
    defaultButton->setText(tr("Defaults"));
    defaultButton->setFixedSize(defaultButton->size());
    connect( defaultButton, SIGNAL(clicked()),this, SLOT(defaults()) );
    defaultButton->move(140,210);

    QPushButton *cancelButton  = new QPushButton(this);
    cancelButton->setText(tr("Cancel"));
    cancelButton->setFixedSize(cancelButton->size());
    connect( cancelButton, SIGNAL(clicked()),this, SLOT(reject()) );
    cancelButton->move(260,210);

    QFrame *separator = new QFrame(this);
    separator->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    separator->setGeometry( 20, 190, 340, 4 );

    for ( int x = 0; x < 4; x++) {
            QLabel *l = new QLabel(this);
            l->setAlignment(AlignCenter);
            labels[x] = l;
    }

    labels[0]->setGeometry(120, 20, 140, 20 );
    labels[1]->setGeometry(120,160, 140, 20 );
    labels[2]->setGeometry( 20, 92, 100, 20 );
    labels[3]->setGeometry(265, 92, 100, 20 );

    QString pixPath;

    QPushButton *up = new QPushButton(this);
    pixPath = FIND_APP_DATA( "pics/up.xpm" );
    up->setPixmap( QPixmap(pixPath));
    up->setFixedSize(up->pixmap()->size());
    connect( up, SIGNAL(clicked()),this, SLOT(butUp()) );
    up->move(180, 50);

    QPushButton *down = new QPushButton(this);
    pixPath = FIND_APP_DATA( "pics/down.xpm");
    down->setPixmap( QPixmap(pixPath));
    down->setFixedSize(down->pixmap()->size());
    connect( down, SIGNAL(clicked()),this, SLOT(butDown()) );
    down->move(180, 130);

    QPushButton *left = new QPushButton(this);
    pixPath = FIND_APP_DATA( "pics/left.xpm");
    left->setPixmap( QPixmap(pixPath));
    left->setFixedSize(left->pixmap()->size());
    connect( left, SIGNAL(clicked()),this, SLOT(butLeft()) );
    left->move(140, 90);

    QPushButton *right = new QPushButton(this);
    pixPath = FIND_APP_DATA( "pics/right.xpm");
    right->setPixmap( QPixmap(pixPath));
    right->setFixedSize(right->pixmap()->size());
    connect( right, SIGNAL(clicked()),this, SLOT(butRight()) );
    right->move(220, 90);


    setCaption(tr("Change Direction Keys"));
    setFixedSize(380, 260);
    lab = 0;
    init();
}

void Keys::keyPressEvent( QKeyEvent *e )
{
        uint kCode = e->key() & ~(SHIFT | CTRL | ALT);
        QString string = KAccel::keyToString(kCode);

        if (lab != 0) {
            if ( string.isNull() )
                    lab->setText(tr("Undefined key"));
            else
                lab->setText(string);
        }
        else if ( lab == 0 && e->key() == Key_Escape)
            reject();
}

void Keys::butUp()
{
    getKey(0);
}

void Keys::butDown()
{
    getKey(1);
}

void Keys::butLeft()
{
    getKey(2);
}

void Keys::butRight()
{
    getKey(3);
}

void Keys::getKey(int i)
{
    if ( lab != 0)
            focusOut(lab);

    focusIn(labels[i]);
}

void Keys::focusOut(QLabel *l)
{
    l->setFrameStyle( QFrame::NoFrame );
    l->setBackgroundColor(backgroundColor());
    l->repaint();
}

void Keys::focusIn(QLabel *l)
{
    lab = l;
    lab->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    lab->setBackgroundColor(white);
    lab->repaint();
}

void Keys::defaults()
{
    if ( lab != 0)
            focusOut(lab);

    lab = 0;

    labels[0]->setText("Up");
    labels[1]->setText("Down");
    labels[2]->setText("Left");
    labels[3]->setText("Right");
}

void Keys::init()
{
    APP_CONFIG_BEGIN( cfg );
    QString up("Up");
    up = cfg->readEntry("upKey", (const char*) up);
    labels[0]->setText(up);

    QString down("Down");
    down = cfg->readEntry("downKey", (const char*) down);
    labels[1]->setText(down);

    QString left("Left");
    left = cfg->readEntry("leftKey", (const char*) left);
    labels[2]->setText(left);

    QString right("Right");
    right = cfg->readEntry("rightKey", (const char*) right);
    labels[3]->setText(right);
    APP_CONFIG_END( cfg );
}

void Keys::ok()
{
    APP_CONFIG_BEGIN( cfg );
    cfg->writeEntry("upKey",   (const char*) labels[0]->text() );
    cfg->writeEntry("downKey", (const char*) labels[1]->text() );
    cfg->writeEntry("leftKey", (const char*) labels[2]->text() );
    cfg->writeEntry("rightKey",(const char*) labels[3]->text() );
    APP_CONFIG_END( cfg );
    accept();
}
