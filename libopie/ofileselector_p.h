#ifndef OPIE_OFILE_SELECTOR_PRIVATE_H
#define OPIE_OFILE_SELECTOR_PRIVATE_H

#include <qmap.h>
#include <qstringlist.h>
#include <qwidget.h>
#include <qlistview.h>

#include <qpe/applnk.h>
#include <qpe/fileselector.h>


/*
 * How to avoid having really two different objects
 * for Extended and ExtendedAll
 * The only difference is the Lister...
 * a) static object?
 * b) leave some object inside the OFileSelector which can be used?
 * c) when switching views tell which view we want o have.. internally we can switch then
 *
 * I'll take c) -zecke
 */


/* the View Interface */
class OFileSelector;
typedef QMap<QString, QStringList> MimeTypes;
class QFileInfo;
class QToolButton;
class OFileViewInterface {
public:
    OFileViewInterface( OFileSelector* selector );
    virtual ~OFileViewInterface();
    virtual QString selectedName()const = 0;
    virtual QString selectedPath()const = 0;
    virtual QString directory()const = 0;
    virtual void reread() = 0;
    virtual int fileCount()const = 0;
    virtual DocLnk selectedDocument()const;
    virtual QWidget* widget( QWidget* parent) = 0;
    virtual void activate( const QString& );
    QString name()const;
protected:
    OFileSelector* selector()const;
    void setName( const QString& );
    bool showNew()const;
    bool showClose()const;
    MimeTypes mimeTypes()const;
    QStringList currentMimeType()const;
    QString startDirectory()const;
protected:
    void ok();
    void cancel();
    void closeMe();
    void fileSelected( const QString& );
    void fileSelected( const DocLnk& );
    void setCurrentFileName( const QString& );
    QString currentFileName()const;

private:
    QString m_name;
    OFileSelector* m_selector;
};


/* THE Document View hosting a FileSelector*/
class ODocumentFileView : public OFileViewInterface {
public:
    ODocumentFileView( OFileSelector* selector );
    ~ODocumentFileView();

    QString selectedName() const;
    QString selectedPath() const;

    QString directory() const;
    void reread();
    int fileCount()const;
    DocLnk selectedDocument()const;

    QWidget* widget( QWidget* parent );

private:
    mutable FileSelector* m_selector;

};

class OFileSelectorItem : public QListViewItem {
public:
    OFileSelectorItem( QListView* view, const QPixmap& pixmap,
                       const QString& path, const QString& date,
                       const QString& size, const QString& mDir,
                       bool isLocked = false, bool isDir = false );
    ~OFileSelectorItem();
    bool isLocked()const;
    bool isDir()const;
    QString directory()const;
    QString path()const;
    QString key(int id, bool )const;

private:
    bool m_locked : 1;
    bool m_isDir    : 1;
    QString m_dir;
};

class OFileViewFileListView : public QWidget {
    Q_OBJECT
public:
    OFileViewFileListView( QWidget* parent, const QString& dir, OFileSelector* selector );
    ~OFileViewFileListView();

    OFileSelectorItem* currentItem()const;
    void reread( bool all = false );
    int fileCount()const;
    QString currentDir()const;
protected:
    bool eventFilter (QObject *o, QEvent *e);
private slots:
    void slotNew(); // will emit newSelected
    void cdUP();
    void cdHome();
    void cdDoc();
    void changeDir( const QString& );
    void slotCurrentChanged( QListViewItem* );
    void slotClicked(int, QListViewItem*, const QPoint&, int );
    void slotFSActivated(int);

protected:

    OFileSelector* selector();

private:
    QMap<QString, QString> m_dev;
    bool m_all : 1;
    OFileSelector* m_sel;
    QPopupMenu* m_fsPop;
    bool compliesMime( const QString& );
    QStringList m_mimes; // used in compy mime
    QString m_currentDir;
    QToolButton *m_btnNew, *m_btnClose;
    void connectSlots();
    void addFile( QFileInfo* info, bool symlink = FALSE );
    void addDir ( QFileInfo* info, bool symlink = FALSE );
    void addSymlink( QFileInfo* info, bool = FALSE );


private:
    QListView* m_view;
};

#endif
