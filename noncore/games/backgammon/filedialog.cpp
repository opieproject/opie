#include "filedialog.h"

#include <qdir.h>
#include <qfileinfo.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qpe/qpeapplication.h>

FileDialog::FileDialog(QWidget* parent,QString header,QString extension,const char* name,bool modal,WFlags f)
        :QDialog(parent,name,modal,f)
{
    setCaption(header);
    ext=extension;
    dirselector=new QListView(this);
    dirselector->setGeometry(1,10,235,200);
    dirselector->addColumn("Files");
    connect(dirselector,SIGNAL(clicked(QListViewItem*)),this,SLOT(selectorclicked(QListViewItem*)));

    getCurrentDir();

    file_name="user";
    fileinput=new QLineEdit(file_name,this);
    fileinput->setGeometry(1,220,235,20);

    QPEApplication::showDialog( this );
}

FileDialog::~FileDialog()
{}


void FileDialog::selectorclicked(QListViewItem* entry)
{
    if(entry==NULL)
        return;
    file_name=entry->text(0);
    fileinput->setText(file_name);
}

void FileDialog::getCurrentDir()
{
    dirselector->clear();
    QDir dir(QPEApplication::qpeDir()+"/backgammon");
    dir.setFilter(QDir::Files);
    QFileInfoListIterator it(*(dir.entryInfoList()));
    QFileInfo* fi;

    int ext_length=ext.length();
    while((fi=it.current())) // go through all file and subdirs
    {
        QString file=fi->fileName();
        if(file.right(ext_length)==ext && file)
        {
            file=file.left(file.length()-ext_length);
            new QListViewItem(dirselector,file);
        }
        ++it;
    }
}

QString FileDialog::filename()
{
    return file_name;
}
