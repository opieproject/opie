#ifndef PK_ITEM_H
#define PK_ITEM_H

#include <qstring.h>
#include <qlistview.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qdict.h>
#include <qobject.h>

class Package //: public QObject
{
  //	Q_OBJECT
 public:
  Package();
  ~Package();
  Package( QStringList );
  Package( QString );
  Package( Package* );
 	
  void    setValue( QString, QString );
  void    copyValues( Package* );

  QString name() ;
  bool    installed();

  void    setDesc( QString );
  QString shortDesc();
  QString desc();
  QString size();
  void    setSection( QString );
  QString getSection();
  QString getSubSection();
  QString details();
  bool    toProcess();
  bool    toInstall();
  bool    toRemove();
public slots:
  void toggleProcess();

private:
  QString _name;
  bool    _toProcess;
  QString _status;
  QString _size;
  QString _section;
  QString _subsection;
  QString _shortDesc;
  QString _desc;
  void parsePackage( QStringList );
};


#endif
