#include <qpe/qpeapplication.h>
#include <qlistbox.h>
#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qmessagebox.h>

#include <opie2/opimmainwindow.h>
#include <opie2/owidgetstack.h>

class mainWindowWidget : public Opie::OPimMainWindow
{
    Q_OBJECT

    public:
        QListBox *notesList;
        QDir fileList;

        mainWindowWidget( QWidget *parent=0, const char *name=0, WFlags fl=0 );
        void refreshList();

    public slots:
        void openFile();
        void deleteFile();

    private:
        int  selected;
        QString fileName;
        QString documentsDirName;

        void editFile(QString filename,  int create);
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
