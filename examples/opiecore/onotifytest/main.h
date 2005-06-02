// (C) Michael 'Mickey' Lauer <mickey@Vanille.de>
// LICENSE = "GPLv2"

#ifndef MAIN_H
#define MAIN_H

/* OPIE */
#include <opie2/ofilenotify.h>

/* QT */
#include <qapplication.h>
#include <qpushbutton.h>
#include <qtextstream.h>

class App : public QApplication
{
  Q_OBJECT
  public:
    App( int argc, char** argv );
   ~App();

  public slots:
    void triggered( const QString&, unsigned int, const QString& );

  private:
    Opie::Core::ODirNotification* tmpfoo;
};

#endif
