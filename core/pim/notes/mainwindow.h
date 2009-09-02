#include <qpe/qpeapplication.h>
#include <qlistbox.h>
#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qmessagebox.h>
#include <notesmanager.h>

#include <opie2/opimmainwindow.h>
#include <opie2/owidgetstack.h>
#include <opie2/opimmemo.h>

namespace Opie {
namespace Notes {
    
class MemoListItem : public QListBoxText
{
public:
    MemoListItem ( QListBox *listbox, const QString &text, int uid );
    int uid();
private:
    int m_uid;
};
    
class mainWindowWidget : public Opie::OPimMainWindow
{
    Q_OBJECT

    public:
        QListBox *notesList;

        mainWindowWidget( QWidget *parent=0, const char *name=0, WFlags fl=0 );
        ~mainWindowWidget();
        void refreshList();

    public slots:
        void openFile();
        void deleteFile();

    private:
        int  selected;
        NotesManager m_manager;
        QString m_curCat;
        
        void editMemo( OPimMemo &memo, bool create );
        void toBeDone(void);
        int currentCatId();
        int currentMemoUid();
        OPimMemo currentMemo();

    private slots:
        void slotItemNew();
        void slotItemEdit();
        void slotItemDuplicate();
        void slotItemDelete();
        void slotItemBeam();
        void slotItemFind();
        void slotConfigure();
        void slotCategoryChanged( const QString& );

    protected slots:
        void flush();
        void reload();
        int  create();
        bool remove( int uid );
        void beam( int uid);
        void show( int uid );
        void edit( int uid );
        void add( const Opie::OPimRecord& );
};

}
}
