#include "newmaildirui.h"
#include <qvariant.h>

class Newmdirdlg : public Newmdirdlgui
{
    Q_OBJECT
public:
    Newmdirdlg( QWidget* parent = 0, const char* name = 0);
    ~Newmdirdlg();
    
    const QString&Newdir()const;
    const bool subpossible()const;

protected slots:
    virtual void accept();
    
protected:
    QString ndir;
    bool possible_subs;
};
