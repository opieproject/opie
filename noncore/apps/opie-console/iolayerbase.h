#ifndef OPIE_IO_LAYER_BASE_H
#define OPIE_IO_LAYER_BASE_H


#include <qwidget.h>

class QLabel;
class QComboBox;
class QVBoxLayout;
class QButtonGroup;
class QRadioButton;
class QHBoxLayout;
class IOLayerBase : public QWidget {

    Q_OBJECT

public:
    enum Flow { Hardware, Software, None };
    enum Parity{ Odd  =2 , Even =1, NonePar =0 };
    enum Speed{ Baud_115200,
                Baud_57600,
                Baud_38400,
                Baud_19200,
                Baud_9600 };
    enum Data { Data_Five =5, Data_Six =6, Data_Seven=7, Data_Eight=8 };
    enum Stop { Stop_One =1, Stop_OnePointFive=15, Stop_Two=2 };

    IOLayerBase( QWidget* base,  const char* name = 0l);
    ~IOLayerBase();

    void setFlow( Flow flo );
    void setParity( Parity par );
    void setSpeed( Speed speed );
    void setData( Data data );
    void setStop( Stop stop );

    Flow flow()const;
    Parity parity()const;
    Speed speed()const;
    Data data()const;
    Stop stop()const;
private:
    QVBoxLayout* m_lroot;
    QLabel* m_speedLabel;
    QComboBox* m_speedBox;
    QButtonGroup* m_groupFlow,  *m_groupData, *m_groupStop ;
    QRadioButton *m_flowHw, *m_flowSw, *m_flowNone;

    QButtonGroup* m_groupParity;
    QRadioButton *m_parityOdd, *m_parityEven, *m_parityNone;
    QRadioButton *m_data5, *m_data6, *m_data7, *m_data8;
    QRadioButton *m_stop1, *m_stop15, *m_stop2;
    QHBoxLayout* m_hbox;
    QHBoxLayout* m_hboxPar;
};


#endif
