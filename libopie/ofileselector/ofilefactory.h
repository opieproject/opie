#ifndef OFILE_FACTORY_H
#define OFILE_FACTORY_H

#include "olister.h"
#include "ofileview.h"
class OFileFactory {
public:
    typedef OLister* (*listerFact)(OFileSelector*);
    typedef OFileView* (*viewFact)(OFileSelector*, QWidget*);
    OFileFactory();
    ~OFileFactory();

    QStringList lister()const;
    QStringList views()const;

    OFileView* view( const QString& name, OFileSelector*, QWidget* );
    OLister* lister( const QString& name, OFileSelector* );

    void addLister( const QString&,  listerFact fact );
    void addView( const QString&, viewFact );

    void removeLister( const QString& );
    void removeView( const QString& );

private:
    QMap<QString, listerFact> m_lister;
    QMap<QString, viewFact> m_view;

};


#endif
