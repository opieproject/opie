
#include "performancetestbase.h"

class PerformanceTest : public PerformanceTestBase
{
    Q_OBJECT
public:
    PerformanceTest( QWidget *parent=0, const char *name=0, WFlags fl=0 );
    static QString appName() { return QString::fromLatin1("performance"); }

protected slots:
    void testClicked( int );
    void doTest();

private:
    QWidget *testWidget;
    QArray<bool> enabledTests;
};

