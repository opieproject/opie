#ifndef OPIE_LOCAL_LISTER
#define OPIE_LOCAL_LISTER

#include "olister.h"

class OLocalLister : public OLister {
public:
    OLocalLister( OFileSelector* );
    ~OLocalLister();
    void reparse( const QString& path );
    QMap<QString, QStringList> mimeTypes(const QString& dir );
    void fileSelected( const QString& dir, const QString& file, const QString& );
    void changedDir( const QString& dir, const QString& file,const QString& );
    QString selectedName()const;
    QStringList selectedNames()const;

private:
    QString m_dir;
};

#endif
