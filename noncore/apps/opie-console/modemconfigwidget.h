#ifndef OPIE_MODEM_CONFIG_WIDGET_H
#define OPIE_MODEM_CONFIG_WIDGET_H

#include "profiledialogwidget.h"
#include "iolayerbase.h"
#include "atconfigdialog.h"

class QVBoxLayout;
class QLabel;
class QComboBox;
class QLineEdit;
class IOLayerBase;
class ModemConfigWidget : public ProfileDialogConnectionWidget {

    Q_OBJECT

public:
    ModemConfigWidget( const QString& name, QWidget* parent, const char* name = 0l );
    ~ModemConfigWidget();

    void load( const Profile& );
    void save( Profile& );
private:
    QVBoxLayout* m_lay;
    QLabel* m_device;
    QComboBox* m_deviceCmb;
    IOLayerBase* m_base;
    QLineEdit* m_telNumber;
    ATConfigDialog *atConf;
private slots:
    void slotAT();
    void slotDial();
};


#endif
