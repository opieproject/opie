
/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify    *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 2 of the License, or       *
 * ( at your option ) any later version.                                   *
 *                                                                         *
 **************************************************************************/
#ifndef _PSEWIDGET_H
#define _PSEWIDGET_H

#include <qwidget.h>

class QGridLayout;

class Element
{
    public:
        Element(const QString &elnum);
        ~Element();

        void setName( QString c){ name = c;};
        void setWeight( QString c){ weight = c;};
        void setDensityName( QString c){density = c;};
        void setGroup( QString c){group = c;};
        void setIE( QString c){ie = c;};
        void setNum( QString c){num = c;};
        void setAZ( QString c){az = c;};
        void setDate( QString c){date = c;};
        void setEN( QString c){en = c;};
        void setMP( QString c){mp = c;};
        void setBP( QString c){bp = c;};
        void setAR( QString c){ar = c;};

        QString elname() const{ return name; };
        QString elweight() const{ return weight; };
        QString eldensity() const{ return density; };
        QString elgroup() const{ return group; };
        QString elie() const{ return ie; };
        QString elnum() const{ return num; };
        QString elaz() const{ return az; };
        QString eldate() const{ return date; };
        QString elen() const{ return en; };
        QString elmp() const{ return mp; };
        QString elbp() const{ return bp; };
        QString elar() const{ return ar; };

    private:
        QString name,     // The name of the element
        weight,           //     weight
        density,          //     density
        group,            //     group
        ie,               //     ionisationenergie
        num,              //     number ( e.g. Carbon is 6 )
        az,               //     state of aggregation
        date,             //     date of discovery
        en,               //     electronegativity
        mp,               //     meltingpoint
        bp,               //     boilingpoint 
        ar;               //     atomic radius
};

class QGridLayout;

class PSEWidget : public QWidget
{
	Q_OBJECT
	
    public:
		PSEWidget();
        QGridLayout *bottom_grid;

    private:
        QGridLayout *maingrid;
        
        void position( int, int&, int& );

    public slots:
        void slotShowElement();

};

#endif
