#include "viewmailbase.h"

/* OPIE */
#include <opie2/oresource.h>
using namespace Opie::Core;

/* QT */
#include <qtextbrowser.h>
#include <qlistview.h>
#include <qaction.h>
#include <qlabel.h>
#include <qvbox.h>
#include <qtoolbar.h>
#include <qlayout.h>
#include <qmenubar.h>

ViewMailBase::ViewMailBase(QWidget *parent, const char *name, WFlags fl)
    : QMainWindow(parent, name, fl)
{
    setCaption(tr("E-Mail view"));
    setToolBarsMovable(false);

    toolbar = new QToolBar(this);
    menubar = new QMenuBar( toolbar );
    mailmenu = new QPopupMenu( menubar );
    menubar->insertItem( tr( "Mail" ), mailmenu );
    toolbar->setHorizontalStretchable(true);
    addToolBar(toolbar);
    QLabel *spacer = new QLabel(toolbar);
    spacer->setBackgroundMode(QWidget::PaletteButton);
    toolbar->setStretchableWidget(spacer);

    reply = new QAction(tr("Reply"), OResource::loadPixmap("mail/reply", OResource::SmallIcon), 0, 0, this);
    reply->addTo(toolbar);
    reply->addTo(mailmenu);

    forward = new QAction(tr("Forward"), OResource::loadPixmap("mail/forward", OResource::SmallIcon), 0, 0, this);
    forward->addTo(toolbar);
    forward->addTo(mailmenu);

    attachbutton = new QAction(tr("Attachments"), OResource::loadPixmap("mail/attach", OResource::SmallIcon), 0, 0, this, 0, true);
    attachbutton->addTo(toolbar);
    attachbutton->addTo(mailmenu);
    connect(attachbutton, SIGNAL(toggled(bool)), SLOT(slotChangeAttachview(bool)));


    showHtml = new QAction( tr( "Show Html" ), OResource::loadPixmap( "mail/html", OResource::SmallIcon ), 0, 0, this, 0, true );
    showHtml->addTo( toolbar );
    showHtml->addTo( mailmenu );

    showPicsInline= new QAction(tr("Show image preview inline"), OResource::loadPixmap("pixmap", OResource::SmallIcon), 0, 0, this);
    showPicsInline->setToggleAction(true);
    showPicsInline->addTo(toolbar);
    showPicsInline->addTo(mailmenu);

    deleteMail = new QAction(tr("Delete Mail"), OResource::loadPixmap("trash", OResource::SmallIcon), 0, 0, this);
    deleteMail->addTo(toolbar);
    deleteMail->addTo(mailmenu);

    QVBox * view = new QVBox(this);
    view->setSpacing(1);
    view->setMargin(0);
    setCentralWidget(view);

    attachments = new QListView(view);
    attachments->setMinimumHeight(90);
    attachments->setMaximumHeight(90);
    attachments->setAllColumnsShowFocus(true);
    attachments->addColumn("Mime Type", 60);
    attachments->addColumn(tr("Description"), 100);
    attachments->addColumn(tr("Filename"), 80);
    attachments->addColumn(tr("Size"), 80);
    attachments->setSorting(-1);

    browser = new QTextBrowser(view);
    adjustSize();
    attachments->hide();
}

void ViewMailBase::slotChangeAttachview(bool state)
{
    if (state) attachments->show();
    else attachments->hide();
}

void ViewMailBase::keyPressEvent ( QKeyEvent * e )
{
    if( e->key()==Qt::Key_Escape ) {
        close();
        e->accept();
        return;
    }
    QWidget::keyPressEvent(e);
}
