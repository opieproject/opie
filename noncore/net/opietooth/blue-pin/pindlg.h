

#include <qdialog.h>

#include "pindlgbase.h"

namespace OpieTooth {
    class PinDlg : public PinDlgBase {
        Q_OBJECT
    public:
        PinDlg(const QString& text,
               const QString& mac,
               QWidget* parent = 0,
               const char* name= 0 );
        ~PinDlg();
        void setMac( const QString& );
        QString pin() const;
    private:
        void test( const QString& mac );
        QString m_mac;
protected slots:
        void accept();
    };


};
