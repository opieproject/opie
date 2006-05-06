

#include <qdialog.h>
#include "pindlgbase.h"

namespace OpieTooth {

    class PinDlg : public PinDlgBase {
        
	Q_OBJECT
	
    public:
        PinDlg(QWidget* parent = 0,const char* name = 0, Qt::WFlags f = 0);

        ~PinDlg();
        void setMac( const QString& );
        QString pin() const;

	static QString appName() { return QString::fromLatin1("bluepin") ; } 
        
    private:
        void test( const QString& mac );

        QString makeTextFromArgs();
        QString makeMacFromArgs();

        QString m_mac;

    protected slots:
        void accept();
        void reject();
        void addnum();
        void delnum();
    };
};
