/****************************************************************************
** Created: Sun Dec 26 22:00:00 2004
**      by:  Paul Eggleton <bluelightning@bluelightning.org>
**     copyright            : (C) 2004 by Paul Eggleton
    email                : bluelightning@bluelightning.org
    *   This program is free software; you can redistribute it and/or modify  *
    *   it under the terms of the GNU General Public License as published by  *
    *   the Free Software Foundation; either version 2 of the License, or     *
    *   (at your option) any later version.                                   *
    ***************************************************************************/

#ifndef DRINKDATA_H
#define DRINKDATA_H

class DrinkItem;

typedef QValueList<DrinkItem> DrinkList;

class DrinkData {
private:
    DrinkList items;
    QString filename;
public:
    DrinkData(void);
    void setFile(const QString &filename);
    bool writeChanges(void);
    bool read(void);
    void addDrink(const QString &name, const QString &ingredients);
    DrinkList::Iterator getBegin(void);
    DrinkList::Iterator getEnd(void);
    DrinkList::Iterator findDrink(const QString &name);
};

class DrinkItem {
private:
    QString name;
    QString ingredients;
public:
    DrinkItem(void);
    DrinkItem(const QString &name, const QString &ingredients);
    const QString &getName(void);
    const QString &getIngredients(void);
    void setName(const QString &name);
    void setIngredients(const QString &ingredients);
};

#endif // DRINKDATA_H
