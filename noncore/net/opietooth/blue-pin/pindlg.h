

#include <qdialog.h>
#include "pindlgbase.h"

namespace OpieTooth {

    class PinDlg : public PinDlgBase {
        
	Q_OBJECT
	
    public:
        PinDlg(const QString& text="",
               const QString& mac="",
               QWidget* parent = 0,
               const char* name= 0 );
        
        PinDlg(QWidget* parent,
               const char* name, 
               Qt::WFlags f );

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

    };
};
