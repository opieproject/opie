#include <qpe/qpeapplication.h>
#include <qlistbox.h>
#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qmessagebox.h>

#include <opie2/opimmainwindow.h>
#include <opie2/owidgetstack.h>
#include <opie2/opimmemo.h>
#include <opie2/omemoaccess.h>

namespace Opie {
namespace Notes {
    
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
        OPimMemoAccess *m_access;
        OPimMemoAccess::List m_notes;
        
        void editMemo( OPimMemo &memo, bool create );
        void toBeDone(void);

    private slots:
        void slotItemNew();
        void slotItemEdit();
        void slotItemDuplicate();
        void slotItemDelete();
        void slotItemBeam();
        void slotItemFind();
        void slotConfigure();

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
