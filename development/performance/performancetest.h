
#include "performancetestbase.h"

class PerformanceTest : public PerformanceTestBase
{
    Q_OBJECT
public:
    PerformanceTest( QWidget *parent=0, const char *name=0, WFlags fl=0 );

protected slots:
    void testClicked( int );
    void doTest();

private:
    QWidget *testWidget;
    QArray<bool> enabledTests;
};

