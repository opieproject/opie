#ifndef MAINWINOWIMP_H
#define MAINWINOWIMP_H

#include "mainwindow.h"
#include <qmap.h>
#include <qstringlist.h>

class Module;
class Interface;
class QLibrary;
class KProcess;
class QCopChannel;
#ifdef QWS
class QLibrary;
#else
class KLibrary;
class KLibLoader;
#define QLibrary KLibrary
#endif

namespace Opie {
namespace Core {
    class OProcess;
}
}

class MainWindowImp : public MainWindow {
  Q_OBJECT

public:
  static QString appName() { return QString::fromLatin1("networksettings"); }
  MainWindowImp(QWidget *parent=0, const char *name=0, WFlags fl = 0);
  ~MainWindowImp();

  QCopChannel *channel;

private slots:
  void getAllInterfaces();

  void addClicked();
  void removeClicked();
  void configureClicked();
  void informationClicked();

  void addProfile();
  void removeProfile();
  void changeProfile();

  void updateInterface(Interface *i);
  void newProfileChanged(const QString& newText);

  void receive (const QCString &, const QByteArray &);

protected slots:
  virtual void setHostname();
  virtual void slotHostname(Opie::Core::OProcess *proc, char *buffer, int buflen);

private:
  void makeChannel();
  void loadModules(const QString &path);
  void initHostname();

  Module* loadPlugin(const QString &pluginFileName,
             const QString &resolveString = "create_plugin");

  // For our local list of names
  QMap<QString, Interface*> interfaceNames;

  QMap<Module*, QLibrary*> libraries;
  QMap<Interface*, QListViewItem*> items;
  QMap<QListViewItem*, Interface*> interfaceItems;
  QStringList m_handledIfaces;

  QMap<KProcess*, QString> threads;
  QStringList profiles;

  bool advancedUserMode;
  QString scheme;
  QString _procTemp;
#ifndef QWS
  KLibLoader *loader;
#endif
};

#endif // MAINWINOWIMP_H

