#ifndef MICROKDE_KFILEDIALOG_H
#define MICROKDE_KFILEDIALOG_H

class KFileDialog
{
  public:
  
    static QString getSaveFileName( const QString &,
                                    const QString &, QWidget * )
    {
      return QString::null;
    }

    static QString getOpenFileName( const QString &,
                                    const QString &, QWidget * )
    {
      return QString::null;
    }
};

#endif
