#include "qcheckmainmenu.h"
#include "qcheckname.h"

QCheckMainMenu::QCheckMainMenu(QWidget *parent)
  : QCheckMMBase(parent)
{
  init();
}

void QCheckMainMenu::init()
{
  lstCheckBooks->clear();
  QString checkdirname = QDir::homeDirPath();
  checkdirname.append("/.checkbooks");
  QDir checkdir(checkdirname);
  if (checkdir.exists() == true)
  {
    QStringList checkbooks = checkdir.entryList("*.qcb", QDir::Files|QDir::Readable|QDir::Writable, QDir::Time);
    for (QStringList::Iterator it = checkbooks.begin(); it != checkbooks.end(); it++)
    {
      (*it) = (*it).remove((*it).find('.'), (*it).length());
    }
    lstCheckBooks->insertStringList(checkbooks);
  }
  lstCheckBooks->clearSelection();
  connect(lstCheckBooks, SIGNAL(clicked(QListBoxItem *)), this, SLOT(slotSelected(QListBoxItem *)));
  lstCheckBooks->clearSelection();
}

void QCheckMainMenu::slotSelected(QListBoxItem *item)
{
  if (item != 0)
  {
    QString text = item->text();
    if (text.isEmpty() == false)
    {
      text.append(".qcb");
      QString checkdirname = QDir::homeDirPath();
      checkdirname.append("/.checkbooks/");
      text.prepend(checkdirname);
      emit itemSelected(text);
    }
  }
}

void QCheckMainMenu::newClicked()
{
  QString checkname = QCheckName::getName();
  if (checkname.isEmpty() == false)
  {
    QString checkdirname = QDir::homeDirPath();
    checkdirname.append("/.checkbooks");
    QDir checkdir(checkdirname);
    if (checkdir.exists() == false)
    {
      checkdir.mkdir(checkdirname);
    }
    checkdirname.append("/");
    checkdirname.append(checkname);
    checkdirname.append(".qcb");
    QFile file(checkdirname);
    if (file.exists() == false)
    {
      file.open(IO_WriteOnly);
      QTextStream os(&file);
      os << "";
      file.close();
    }
    QFileInfo fi(file);
    QString noextension = fi.fileName();
    noextension = noextension.remove(noextension.find('.'), noextension.length());
    lstCheckBooks->insertItem(noextension);
  }
}

void QCheckMainMenu::deleteClicked()
{
    QString checkname = lstCheckBooks->currentText();
    if (checkname.isEmpty() == false)   {
        switch ( QMessageBox::warning(this,tr("Delete Account"),tr("Really delete the\n")
                                      +checkname+tr(" account?")
                                      ,tr("Yes"),tr("No"),0,1,1) ) {
          case 0: {

              QString checkdirname = QDir::homeDirPath();
              checkdirname.append("/.checkbooks");
              QDir checkdir(checkdirname);
              QString checkDir=checkdirname;
              
              checkdirname.append("/");
              checkdirname.append(checkname);
              checkdirname.append(".qcb");
              QFile file(checkdirname);
              if (file.exists() == true) {
                  if(!file.remove()) {
                      QMessageBox::message(tr("Checkbook"),tr("Could not remove account"));
                      return;
                  } else
                      lstCheckBooks->removeItem(lstCheckBooks->currentItem());
              }
          }
              break;
        }
    }
}
