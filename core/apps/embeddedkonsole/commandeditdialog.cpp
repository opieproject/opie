//comandeditdialog.cpp

#include "commandeditdialog.h"
#include "playlistselection.h"
#include <qstring.h>
#include <qpe/config.h>
#include <qpe/qpetoolbar.h>
#include <qwidget.h>
#include <qpe/qpemenubar.h>
#include <qpe/resource.h>
#include <qlist.h>
#include <qtoolbutton.h>
#include <qvbox.h>
#include <qlistview.h>
#include <qlineedit.h>
#include <qheader.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include "smallcommandeditdialogbase.h"

CommandEditDialog::CommandEditDialog(QWidget *parent, const char* name, WFlags fl )
    : CommandEditDialogBase(parent, name, TRUE, fl)

{
  m_SuggestedCommandList->addColumn( tr("Command Selection") );
  m_SuggestedCommandList->header()->hide();
  m_SuggestedCommandList->setSorting(-1,FALSE);
  m_SuggestedCommandList->clearSelection();
  m_SuggestedCommandList->setSorting(0,TRUE);

    QListViewItem *item;


  item = new QListViewItem( m_SuggestedCommandList,"ls ");

  item = new QListViewItem( m_SuggestedCommandList,"cat ");
  item = new QListViewItem( m_SuggestedCommandList,"cd ");
  item = new QListViewItem( m_SuggestedCommandList,"chmod ");
  item = new QListViewItem( m_SuggestedCommandList,"cp ");
  item = new QListViewItem( m_SuggestedCommandList,"dc ");
  item = new QListViewItem( m_SuggestedCommandList,"df ");
  item = new QListViewItem( m_SuggestedCommandList,"dmesg ");
  item = new QListViewItem( m_SuggestedCommandList,"echo ");
  item = new QListViewItem( m_SuggestedCommandList,"export ");
  item = new QListViewItem( m_SuggestedCommandList,"env ");
  item = new QListViewItem( m_SuggestedCommandList,"find ");
  item = new QListViewItem( m_SuggestedCommandList,"free ");
  item = new QListViewItem( m_SuggestedCommandList,"ifconfig ");
  item = new QListViewItem( m_SuggestedCommandList,"ipkg ");
  item = new QListViewItem( m_SuggestedCommandList,"mkdir ");
  item = new QListViewItem( m_SuggestedCommandList,"mv ");
  item = new QListViewItem( m_SuggestedCommandList,"nc localhost 7776 ");
  item = new QListViewItem( m_SuggestedCommandList,"nc localhost 7777 ");
  item = new QListViewItem( m_SuggestedCommandList,"nslookup ");
  item = new QListViewItem( m_SuggestedCommandList,"ping ");
  item = new QListViewItem( m_SuggestedCommandList,"ps aux");
  item = new QListViewItem( m_SuggestedCommandList,"pwd ");
  item = new QListViewItem( m_SuggestedCommandList,"rm ");
  item = new QListViewItem( m_SuggestedCommandList,"rmdir ");
  item = new QListViewItem( m_SuggestedCommandList,"route ");
  item = new QListViewItem( m_SuggestedCommandList,"gzip ");
  item = new QListViewItem( m_SuggestedCommandList,"gunzip ");
  item = new QListViewItem( m_SuggestedCommandList,"chgrp ");
  item = new QListViewItem( m_SuggestedCommandList,"chown ");
  item = new QListViewItem( m_SuggestedCommandList,"date ");
  item = new QListViewItem( m_SuggestedCommandList,"dd ");
  item = new QListViewItem( m_SuggestedCommandList,"df ");
  item = new QListViewItem( m_SuggestedCommandList,"dmesg ");
  item = new QListViewItem( m_SuggestedCommandList,"fuser ");
  item = new QListViewItem( m_SuggestedCommandList,"hostname ");
  item = new QListViewItem( m_SuggestedCommandList,"kill ");
  item = new QListViewItem( m_SuggestedCommandList,"killall ");
  item = new QListViewItem( m_SuggestedCommandList,"ln ");
  item = new QListViewItem( m_SuggestedCommandList,"ln -s ");
  item = new QListViewItem( m_SuggestedCommandList,"mount ");
  item = new QListViewItem( m_SuggestedCommandList,"more ");
  item = new QListViewItem( m_SuggestedCommandList,"sort ");
  item = new QListViewItem( m_SuggestedCommandList,"touch ");
  item = new QListViewItem( m_SuggestedCommandList,"umount ");
  item = new QListViewItem( m_SuggestedCommandList,"mknod ");
  item = new QListViewItem( m_SuggestedCommandList,"netstat ");
  item = new QListViewItem( m_SuggestedCommandList,"cardctl eject ");
  m_SuggestedCommandList->setSelected(m_SuggestedCommandList->firstChild(),TRUE);
  m_SuggestedCommandList->sort();

  connect( m_SuggestedCommandList, SIGNAL( clicked( QListViewItem * ) ), m_PlayListSelection, SLOT( addToSelection( QListViewItem *) ) );



  ToolButton1->setTextLabel("new");
  ToolButton1->setPixmap(Resource::loadPixmap("new"));
  ToolButton1->setAutoRaise(TRUE);
  ToolButton1->setFocusPolicy(QWidget::NoFocus);
  connect(ToolButton1,SIGNAL(clicked()),this,SLOT(showAddDialog()));

  ToolButton2->setTextLabel("edit");
  ToolButton2->setPixmap(Resource::loadPixmap("edit"));
  ToolButton2->setAutoRaise(TRUE);
  ToolButton2->setFocusPolicy(QWidget::NoFocus);
  connect(ToolButton2,SIGNAL(clicked()),this,SLOT(showEditDialog()));

  ToolButton3->setTextLabel("delete");
  ToolButton3->setPixmap(Resource::loadPixmap("editdelete"));
  ToolButton3->setAutoRaise(TRUE);
  ToolButton3->setFocusPolicy(QWidget::NoFocus);
  connect(ToolButton3,SIGNAL(clicked()),m_PlayListSelection,SLOT(removeSelected()));

  ToolButton4->setTextLabel("up");
  ToolButton4->setPixmap(Resource::loadPixmap("up"));
  ToolButton4->setAutoRaise(TRUE);
  ToolButton4->setFocusPolicy(QWidget::NoFocus);
  connect(ToolButton4,SIGNAL(clicked()),m_PlayListSelection,SLOT(moveSelectedUp()));

  ToolButton5->setTextLabel("down");
  ToolButton5->setPixmap(Resource::loadPixmap("down"));
  ToolButton5->setAutoRaise(TRUE);
  ToolButton5->setFocusPolicy(QWidget::NoFocus);

connect(ToolButton5,SIGNAL(clicked()),m_PlayListSelection,SLOT(moveSelectedDown()));




  QListViewItem *current = m_SuggestedCommandList->selectedItem();
    if ( current )
        item->moveItem( current );
    m_SuggestedCommandList->setSelected( item, TRUE );
    m_SuggestedCommandList->ensureItemVisible( m_SuggestedCommandList->selectedItem() );
  Config cfg("Konsole");
  cfg.setGroup("Commands");
  if (cfg.readEntry("Commands Set","FALSE") == "TRUE") {
   for (int i = 0; i < 100; i++) {
     QString tmp;
     tmp = cfg.readEntry( QString::number(i),"");
     if (!tmp.isEmpty())
         m_PlayListSelection->addStringToSelection(tmp);
   }
  } else {

m_PlayListSelection->addStringToSelection("ls ");
m_PlayListSelection->addStringToSelection("cardctl eject");
m_PlayListSelection->addStringToSelection("cat ");
m_PlayListSelection->addStringToSelection("cd ");
m_PlayListSelection->addStringToSelection("chmod ");
m_PlayListSelection->addStringToSelection("cp ");
m_PlayListSelection->addStringToSelection("dc ");
m_PlayListSelection->addStringToSelection("df ");
m_PlayListSelection->addStringToSelection("dmesg");
m_PlayListSelection->addStringToSelection("echo ");
m_PlayListSelection->addStringToSelection("env");
m_PlayListSelection->addStringToSelection("find ");
m_PlayListSelection->addStringToSelection("free");
m_PlayListSelection->addStringToSelection("grep ");
m_PlayListSelection->addStringToSelection("ifconfig ");
m_PlayListSelection->addStringToSelection("ipkg ");
m_PlayListSelection->addStringToSelection("mkdir ");
m_PlayListSelection->addStringToSelection("mv ");
m_PlayListSelection->addStringToSelection("nc localhost 7776");
m_PlayListSelection->addStringToSelection("nc localhost 7777");
m_PlayListSelection->addStringToSelection("nslookup ");
m_PlayListSelection->addStringToSelection("ping ");
m_PlayListSelection->addStringToSelection("ps aux");
m_PlayListSelection->addStringToSelection("pwd ");
m_PlayListSelection->addStringToSelection("rm ");
m_PlayListSelection->addStringToSelection("rmdir ");
m_PlayListSelection->addStringToSelection("route ");
m_PlayListSelection->addStringToSelection("set ");
m_PlayListSelection->addStringToSelection("traceroute");

}
}
CommandEditDialog::~CommandEditDialog()
{
}

void CommandEditDialog::accept()
{
int i = 0;
  Config *cfg = new Config("Konsole");
  cfg->setGroup("Commands");
  cfg->clearGroup();

    QListViewItemIterator it( m_PlayListSelection );

    for ( ; it.current(); ++it ) {
//    qDebug(it.current()->text(0));
    cfg->writeEntry(QString::number(i),it.current()->text(0));
    i++;

    }
    cfg->writeEntry("Commands Set","TRUE");
//    qDebug("CommandEditDialog::accept() - written");
    delete cfg;
    emit commandsEdited();
    close();





}

void CommandEditDialog::showEditDialog()
{
editCommandBase *d = new editCommandBase(this,"smalleditdialog", TRUE);
d->setCaption("Edit command");
d->TextLabel->setText("Edit command:");
d->commandEdit->setText(m_PlayListSelection->currentItem()->text(0));
int i = d->exec();
if ((i==1) && (!(d->commandEdit->text()).isEmpty()))
  m_PlayListSelection->currentItem()->setText(0,(d->commandEdit->text()));
}

void CommandEditDialog::showAddDialog()
{

editCommandBase *d = new editCommandBase(this,"smalleditdialog", TRUE);
int i = d->exec();
if ((i==1) && (!(d->commandEdit->text()).isEmpty()))
m_PlayListSelection->addStringToSelection(d->commandEdit->text());

}

