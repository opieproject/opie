#include <qpalette.h>
#include <qcolor.h>


#include "stationviewitem.h"
#include "opiestumbler.h"

StationViewItem::StationViewItem( QListView *parent, const QString &ssid,
        const QString &channel, const QString &level, const QString &enc,
        const QString &mac )
    : QListViewItem( parent, ssid, channel, level, enc, mac )
{
}

StationViewItem::StationViewItem( QListView *parent, const QString &ssid,
        const QString &channel, const QString &level, const QString &enc,
        const QString &vendor, const QString &mac )

: QListViewItem( parent, ssid, channel, level, enc, vendor, mac )
{
}

void StationViewItem::paintCell( QPainter *p, const QColorGroup &cg,
                            int column, int width, int alignment )
{
    QColor c;
    QColorGroup ch(cg);

    if ( column == OpieStumbler::CURSIGNAL ) {
        int value = text(column).toInt();
        if ( value < 20 )
            c.setRgb(255, 0, 0);
        else if ( value < 35 )
            c.setRgb(255, 255, 0);
        else if ( value < 60 )
            c.setRgb(0, 255, 0);
        else
            c.setRgb(0, 0, 255);
        ch.setColor( QColorGroup::Text, c );
    }

    if ( column == OpieStumbler::CURSSID ) {
        if ( text(OpieStumbler::CURENC) == "Y" )
            c.setRgb(255, 0, 0);
        else
            c.setRgb(0, 255, 0);
        ch.setColor( QColorGroup::Text, c );
    }

    QListViewItem::paintCell( p, ch, column, width, alignment );
}

