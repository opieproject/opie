#ifndef STATIONVIEWITEM_H
#define STATIONVIEWITEM_H

#include <qlistview.h>


class QString;
class QPainter;
class QColorGroup;
class QColor;

class  StationViewItem: public QListViewItem
{
public:
    StationViewItem( QListView *parent, const QString &ssid, const QString &channel, 
            const QString &level, const QString &enc, const QString &mac );
    StationViewItem( QListView *parent, const QString &ssid, const QString &channel, 
            const QString &level, const QString &enc, const QString &vendor,
            const QString &mac );
    
    virtual void paintCell( QPainter *p, const QColorGroup &cg,
                            int column, int width, int alignment );
};

#endif

