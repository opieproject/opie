/* OPIE */
#include <opie2/olistview.h>
#include <opie2/odebug.h>
#include <opie2/oapplication.h>
#include <opie2/ofilenotify.h>

class QButtonGroup;

class DemoApp : public Opie::Core::OApplication
{
  Q_OBJECT
public:
  DemoApp( int argc, char** argv );

public:
    void addTrigger( bool multi = false );
public slots:
    void modifierClicked( int modifier );
    void addSingle();
    void addMulti();
    void delTrigger();
    void trigger();

private:
  Opie::Ui::OListView* l;
  QButtonGroup* g1;
  QButtonGroup* g2;
  Opie::Core::OFileNotificationType m;
};
