#ifndef OPIE_BT_CONFIG_WIDGET_H
#define OPIE_BT_CONFIG_WIDGET_H

#include "profiledialogwidget.h"

class QVBoxLayout;
class QLabel;
class QComboBox;
class QLineEdit;
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

};


#endif
