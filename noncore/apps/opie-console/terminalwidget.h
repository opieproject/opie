#ifndef OPIE_TERMINAL_WIDGET_H
#define OPIE_TERMINAL_WIDGET_H

#include "profiledialogwidget.h"

class QComboBox;
class QLabel;
class QVBoxLayout;
class QHBoxLayout;
class QButtonGroup;
class QRadioButton;
class QCheckBox;
class QHGroupBox;

class TerminalWidget : public ProfileDialogTerminalWidget {
    Q_OBJECT
public:
    TerminalWidget(const QString& name, QWidget* wid,
                   const char* na ) ;
    ~TerminalWidget();

    void load( const Profile& );
    void save( Profile& );
private:
    QVBoxLayout* m_lroot,  *m_typeBox, *m_colorBox;

    QHBoxLayout* m_hbox;

    QLabel* m_terminal,  *m_colorLabel;

    QComboBox* m_terminalBox, *m_colorCmb;

    QButtonGroup* m_groupSize;

    QRadioButton* m_sizeSmall, *m_sizeMedium,
        *m_sizeLarge;

    QHGroupBox *m_groupConv, *m_groupOptions;

    QCheckBox *m_convInbound,
        *m_convOutbound, *m_optionEcho,
        *m_optionWrap;

};

#endif
