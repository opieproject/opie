/*
 * stocktickerconfig.h
 *
 * copyright   : (c) 2002 by LJP
 * email       : llornkcor@handhelds.org
 *
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STOCKTICKER_PLUGIN_CONFIG_H
#define STOCKTICKER_PLUGIN_CONFIG_H

#include <qwidget.h>
#include <opie/todayconfigwidget.h>
#include <qstring.h>

class QLineEdit;
class QCheckBox;
class QPushButton;
class QCheckBox;
class QSpinBox;

class StocktickerPluginConfig : public TodayConfigWidget {
//Q_OBJECT
public:
    StocktickerPluginConfig( QWidget *parent,  const char *name );
    ~StocktickerPluginConfig();
    QString text() const;
    void writeConfig();
private:
    QLineEdit* LineEdit1;
    QCheckBox *timeCheck, *dateCheck, *symbolCheck, *nameCheck, *currentPriceCheck, *lastPriceCheck, *openPriceCheck, *minPriceCheck, *maxPriceCheck, *variationCheck, *volumeCheck;
    QPushButton *lookupButton;
    QSpinBox *timerDelaySpin;
private slots:
    void doLookup();

};
#endif
