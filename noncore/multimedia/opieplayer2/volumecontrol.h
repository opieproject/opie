/*************
 * this is only a quick hack and will be later replaced by osound
 *
 **********/


#ifndef VOLUMECONTROL_H
#define VOLUMECONTROL_H



#include <qobject.h>

class VolumeControl : public QObject {
    Q_OBJECT
public:
    VolumeControl();
    ~VolumeControl();

    // increase by "ammount"
    void incVol( int ammount );
    void decVol( int ammount );

    /**
     * Get the volume in percent
     * @return volume percentage
     */
    int getVolume();

public slots:

    /**
     * Set the volume in percent
     * @value volumePerc between 0 and 100
     */
    void setVolume( int volumePerc );



private:

    int m_volumePerc;

};

#endif

