/*
 *  OPIE Notes
 *
 *  based on NoteZ 1.1.0 by Henning Holtschneider <hh@holtschneider.com>
 *
 *  moved to OPIE Pim framework by Marcin Juszkiewicz <openembedded@hrw.one.pl>
 *
 *  History:
 *  - 2005.10.12 - started work
 *  - 2005.10.19 - version 0.2:
 *                 - first working version
 *                 - info sent to NoteZ author and to OPIE devel
 *
 *  - 2005.10.20 - version 0.3:
 *                 - load/save in UTF-8
 *                 - load all files from Documents/text/plain
 *                 - create .txt files not .ntz - timestamp used as name
 *                 - one variable (documentsDirName) keep location of notes
 *                 - code (re)indented and converted to spaces
 *                 - variables translated to English (were in German)
 *                 - started work on beaming
 *
 *  - 2005.10.26 - version 0.4:
 *                 - added check does Documents/text/plain exist and create it if not
 *
 *  ToDo:
 *  - beaming
 *  - moving to SQLite database
 *  - category support
 *  - searching
 *
 */

#include "mainwindow.h"
#include "editwindow.h"
#include <qpe/ir.h>

mainWindowWidget::mainWindowWidget( QWidget *parent, const char *name, WFlags)
    : Opie::OPimMainWindow( "Notes", "Notes", tr( "Note" ), "notes",
            parent, name, WType_TopLevel | WStyle_ContextHelp )
{
    setCaption( tr("Notes"));
    notesList = new QListBox(this, "notesList");
    setCentralWidget(notesList);

    documentsDirName = QPEApplication::documentDir() + "/text/plain/";

    fileList.setPath(documentsDirName);

    if(!fileList.exists())
    {
        fileList.setPath(QPEApplication::documentDir() + "/text/");

        if(!fileList.exists())
        {
            QString text;

            if(!fileList.mkdir(fileList.absPath()))
            {
                QMessageBox::critical(0, tr("i/o error"), text.sprintf(tr("Could not create directory '%s'"), fileList.absPath()));
            }
            else
            {
                fileList.setPath(documentsDirName);

                if(!fileList.mkdir(fileList.absPath()))
                {
                    QMessageBox::critical(0, tr("i/o error"), text.sprintf(tr("Could not create directory '%s'"), fileList.absPath()));
                }
            }
        }
    }

    this->selected = -1;
    refreshList();

    QObject::connect(notesList, SIGNAL(returnPressed(QListBoxItem*)), this, SLOT(slotItemEdit()));
    QObject::connect(notesList, SIGNAL(doubleClicked(QListBoxItem*)), this, SLOT(slotItemEdit()));

    initBars();
}

void mainWindowWidget::deleteFile()
{
    if( notesList->count() > 0 )
    {
        switch (QMessageBox::warning(0, tr("Delete note"), tr("Really delete\n'") + notesList->currentText() + "'?",
                    QMessageBox::Yes, QMessageBox::No))
        {
            case QMessageBox::Yes:
                this->selected = notesList->currentItem();
                QFile::remove(documentsDirName + fileList[notesList->currentItem()]);
                refreshList();
                break;

            case QMessageBox::No:
                // don't delete
                break;
        }
    }
}

void mainWindowWidget::editFile(QString filename, int create)
{
    editWindowWidget *editWindow = new editWindowWidget(0, "editWindow", true);

    editWindow->loadFile(filename);

    if(QPEApplication::execDialog(editWindow) == QDialog::Accepted)
    {
        editWindow->saveFile(filename);
        if( create )
        {
            // the new selection will be always at the end and count is already
            // 1 bigger than selected
            this->selected = notesList->count();
        }
    }

    refreshList();
}

int mainWindowWidget::create()
{
    QString name;
    int now = time(0);

    this->editFile(name.sprintf(documentsDirName + "%d.txt", now), true );

    return 0;   //FIXME
}

void mainWindowWidget::slotItemEdit()
{
    openFile();
}

void mainWindowWidget::slotItemDelete()
{
    deleteFile();
}

void mainWindowWidget::slotItemNew()
{
    create();
}

void mainWindowWidget::slotItemDuplicate()
{
    QString fileName = documentsDirName + fileList[notesList->currentItem()];
    int now = time(0);

    QFile fileOld(fileName);

    if (fileOld.exists())
    {
        if (!fileOld.open(IO_ReadOnly))
        {
            QMessageBox::warning(0, tr("File i/o error"), fileName.sprintf(tr("Could not read file '%s'"), fileName));
        }
        else
        {
            QFile fileNew(documentsDirName + fileName.sprintf("%d.txt", now));

            if (!fileNew.exists())
            {
                if(fileNew.open(IO_WriteOnly))
                {
                    QTextStream inStream(&fileOld);
                    inStream.setEncoding(QTextStream::UnicodeUTF8);

                    QTextStream outStream(&fileNew);
                    outStream.setEncoding(QTextStream::UnicodeUTF8);
                    outStream << inStream.read();

                    fileOld.close();
                    fileNew.close();
                    refreshList();
                }
            }
        }
    }
}

void mainWindowWidget::openFile()
{
    int number = notesList->currentItem();

    if( notesList->count() > 0 )
    {
        this->selected = number;
        this->editFile(documentsDirName + fileList[number], false);
    }
}

void mainWindowWidget::refreshList()
{
    unsigned int item;
    QString title;

    notesList->clear();

    fileList.setPath(documentsDirName);
    fileList.setFilter(QDir::Files);
    fileList.setSorting(QDir::Name);

    for (item = 0; item < fileList.count(); item++)
    {
        QFile file(documentsDirName + fileList[item]);

        if (!file.open(IO_ReadOnly))
        {
            QMessageBox::warning(0, tr("File i/o error"), title.sprintf(tr("Could not read file '%s'"), fileList[item]));
        }
        else
        {
            QTextStream inStream(&file);
            inStream.setEncoding(QTextStream::UnicodeUTF8);

            if (!inStream.atEnd())
            {
                title = inStream.readLine();
            }
            else
            {
                title = tr("<empty file>");
            }

            if (title.length() < 1)
            {
                title = tr("<no caption>");
            }

            file.close();

            notesList->insertItem(title);
        }
    }

    if( notesList->count() > 0 )
    {
        if( this->selected == -1 )
        {
            notesList->setCurrentItem( 0 );
        }
        else
        {
            if( notesList->count() > this->selected  )
            {
                notesList->setCurrentItem( this->selected );
            }
            else
            {
                notesList->setCurrentItem( notesList->count() - 1 );
            }

        }
    }
    else
    {
        this->selected = -1;
    }

}

void mainWindowWidget::slotItemBeam()
{
    Ir obex;

    obex.send(documentsDirName + fileList[notesList->currentItem()], "test", "text/plain");
}

void mainWindowWidget::slotItemFind()           { toBeDone();}
void mainWindowWidget::slotConfigure()          { toBeDone();}
void mainWindowWidget::flush()                  { toBeDone();}
void mainWindowWidget::reload()                 { toBeDone();}
bool mainWindowWidget::remove( int /*uid*/ )    { toBeDone(); return false; }
void mainWindowWidget::beam(   int /*uid*/ )    { toBeDone();}
void mainWindowWidget::show(   int /*uid*/ )    { toBeDone();}
void mainWindowWidget::edit(   int /*uid*/ )    { toBeDone();}
void mainWindowWidget::add( const Opie::OPimRecord& ) { toBeDone();}

void mainWindowWidget::toBeDone(void)
{
    QMessageBox::information( this, "Notes", tr("Not yet implemented"));
}

