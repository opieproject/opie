
#ifndef OpieToothDevice_H
#define OpieToothDevice_H

#include <qobject.h>
#include <qstring.h>
#include <qvaluelist.h>

namespace OpieTooth {
  class Device : public QObject {
    Q_OBJECT
  public:
    Device(const QString &device, const QString& mode);
    // unloads the device
    ~Device();
    // was the device loaded?
    void attach();
    void detach();
    bool up()const;
    QString devName()const ; // hci0
  signals:    
    device(const QString& device, bool up );
  };
};



#endif
