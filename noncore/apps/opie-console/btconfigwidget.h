#ifndef OPIE_BT_CONFIG_WIDGET_H
#define OPIE_BT_CONFIG_WIDGET_H

#include "profiledialogwidget.h"

class QVBoxLayout;
class QLabel;
class QComboBox;
class QLineEdit;
class QRadioButton;
class IOLayerBase;
class BTConfigWidget : public ProfileDialogConnectionWidget {

    Q_OBJECT

public:
    BTConfigWidget( const QString& name, QWidget* parent, const char* name = 0l );
    ~BTConfigWidget();

    void load( const Profile& );
    void save( Profile& );
private:
    QVBoxLayout* m_lay;
    QLabel* m_device;
    QComboBox* m_deviceCmb;
    IOLayerBase* m_base;
    QLineEdit* m_mac;
    QRadioButton *m_macRadio;
    QRadioButton *m_devRadio;

private slots:
    void slotMacRadio( bool on );
    void slotDevRadio( bool on );
};


#endif
