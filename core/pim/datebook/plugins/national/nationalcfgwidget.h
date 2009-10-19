#ifndef NATIONALHOLIDAYCONFIGWIDGET_H
#define NATIONALHOLIDAYCONFIGWIDGET_H

#include <opie2/oholidayplugincfgwidget.h>

#include <qvariant.h>
#include <qwidget.h>
#include <qstringlist.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QListView;
class QListViewItem;

class NationalHolidayConfigWidget : public Opie::Datebook::HolidayPluginConfigWidget
{
    Q_OBJECT

public:
    NationalHolidayConfigWidget( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    virtual ~NationalHolidayConfigWidget();

    virtual void saveConfig();

protected slots:
    virtual void listItemClicked(QListViewItem*);

protected:
    void init();

    QVBoxLayout* NationalHolidayConfigLayout;
    QLabel* m_headLabel;
    QListView* m_Configlist;
    QStringList files;
};

#endif // NATIONALHOLIDAYCONFIG_H
