/****************************************************************************
** Form interface generated from reading ui file 'zsafe.ui'
**
** Created: Sat Apr 6 17:57:42 2002
**
** Author: Carsten Schneider <CarstenSchneider@t-online.de>
**
** $Id: zsafe.h,v 1.2 2004-02-17 23:47:39 alwin Exp $
**
** Homepage: http://home.t-online.de/home/CarstenSchneider/zsafe/index.html
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#ifndef ZSAFE_H
#define ZSAFE_H

#include <stdio.h>

#include "infoform.h"
#include "categorylist.h"
#include "shadedlistitem.h"

#include <qvariant.h>
#include <qdialog.h>
#include <qwidgetstack.h>
#include <qtimer.h>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class ZListView;
class QListViewItem;
class QToolButton;
class QTextBrowser;
class QPopupMenu;
#ifdef DESKTOP
// #ifndef WIN32
class QSettings;
// #endif
#else
class Config;
#endif
class CategoryDialog;
class QPixmap;

// number of fields for one entry
#define FIELD_SIZE 7

class ZSafe : public QDialog
{ 
    Q_OBJECT

protected:
    void paintEvent( QPaintEvent * e );
    void resizeEvent ( QResizeEvent * );

    bool raiseFlag;
    QTimer raiseTimer;

public:
    ZSafe( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = WDestructiveClose );
    ~ZSafe();

    static const QColor *evenRowColor;
    static const QColor *oddRowColor;

    QToolButton* Edit;
    QToolButton* Delete;
    QToolButton* Find;
    QToolButton* New;
    ZListView* ListView;

    QString cfgFile;
    QString iconPath;

    InfoForm *infoForm;
    CategoryDialog *categoryDialog;

    QListViewItem *selectedItem; // current selected item
    QListViewItem *copiedItem; // copied item
    bool IsCopy;
    bool IsCut;
    bool modified; // true if database is modified

    QListViewItem *lastSearchedCategory; 
    QListViewItem *lastSearchedItem; 
    QString        lastSearchedName;
    QString        lastSearchedUsername;
    QString        lastSearchedComment;
   

    CategoryList categories; // list of categories

    bool isCategory(QListViewItem *_item);
    void getDocPassword(QString title);
    QString filename;
   
    enum {
                PWERR_GOOD,     // success
                PWERR_OPEN,     // can't open filedescriptor / can't create file
                PWERR_PERM,     // permissions are bad
                PWERR_SYML,     // is a symlink
                PWERR_STAT,     // can't get file status
                PWERR_DATA      // can't read correct data
    };

    long load_buffer_length;
    long save_buffer_length;

    /** password for the file */
    QString m_password;
    /** globals needed for file loading */
    int lastcount, size;
    /** these three are needed in all save functions and initialized in save _init() */
    FILE *fd;
    unsigned short iv[4];
    char *buffer;
    /** these two are global because save_entry() and save_finalize() both n eed them */
    int bufferIndex;
    unsigned short plaintext[4];

#ifdef DESKTOP
// #ifndef WIN32
    QSettings *conf;
// #endif
#else
    Config *conf;
#endif
    bool expandTree;
    QPopupMenu *file;

    QTimer docuTimer;


    bool openDocument(const char* filename, const char* format=0);
    int loadInit(const char* filename, const char *password);
    int loadEntry(char *entry[4]);
    int loadFinalize(void);
    bool saveDocument(const char* filename, bool withPwd, const char* format=0);
    int saveInit(const char *filename, const char *password);
    int saveEntry(char *entry[4]);
    int saveFinalize(void);
    void saveConf();
    QPixmap * getPredefinedIcon(QString category);
    QString getFieldLabel (QListViewItem *_item, QString field, QString def);
    QString getFieldLabel (QString category, QString field, QString def);
    void setCategoryDialogFields(CategoryDialog *dialog);
    void setCategoryDialogFields(CategoryDialog *dialog, QString category);
    void saveCategoryDialogFields(CategoryDialog *dialog);


    void resume(int signum);
    void exitZs (int ec);


public slots:
    virtual void deletePwd();
    virtual void editPwd();
    virtual void newPwd();
    virtual void findPwd();
    virtual void quitMe();
    virtual void listViewSelected( QListViewItem *_item);
    virtual void showInfo( QListViewItem *_item);
    virtual void writeAllEntries();
    virtual void readAllEntries();
    virtual void removeAsciiFile();
    virtual void setPasswordDialogDone();

    virtual void addCategory();
    virtual void delCategory();
    virtual void editCategory();
    virtual void cutItem();
    virtual void copyItem();
    virtual void pasteItem();

    virtual void newDocument();
    virtual void loadDocument();
    virtual void saveDocumentAs();
    virtual void saveDocumentWithoutPwd();
    virtual void saveDocumentWithPwd();
    virtual void about();
    virtual void setExpandFlag();

    virtual void categoryFieldActivated( const QString& str);

private slots:
    void setDocument(const QString& fileref);
    void slotLoadDocu();
    void slotRaiseTimer();

};

#endif // ZSAFE_H
