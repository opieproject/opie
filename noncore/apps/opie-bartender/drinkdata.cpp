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

/* QT */
#include <qfile.h>
#include <qtextstream.h>
#include <qvaluelist.h>

/* STD */
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include "drinkdata.h"

// ----- DrinkData -----

DrinkData::DrinkData() {
}

void DrinkData::setFile(const QString &filename) {
    this->filename = filename;
}

bool DrinkData::read(void) {
    QFile dbFile;
    
    dbFile.setName(filename);
    if ( !dbFile.open( IO_ReadOnly)) {
        return false;
    }
    
    items.clear();
    
    QTextStream t( &dbFile);
    QString s, name = "", ingredients = "";
    
    while ( !t.eof()) {
        s = t.readLine();
        if(s.find( "#", 0, TRUE) != -1 || dbFile.atEnd()) {
            if(name != "")
                addDrink(name, ingredients);
            // Start new entry
            name = s.right(s.length()-2);
            ingredients = "";
        }
        else {
            if(ingredients != "")
                ingredients += '\n';
            ingredients += s;
        }
    }
    
    dbFile.close(); 

    return true;
}

void DrinkData::addDrink(const QString &name, const QString &ingredients) {
    DrinkItem item(name, ingredients);
    items.append(item);
}

DrinkList::Iterator DrinkData::findDrink(const QString &name) {
    DrinkList::Iterator it = items.begin();
    while ( it != items.end() ) {
        if((*it).getName() == name)
            return it;
        ++it;
    }
    return items.end();
}

DrinkList::Iterator DrinkData::getBegin(void) {
    return items.begin();
}

DrinkList::Iterator DrinkData::getEnd(void) {
    return items.end();
}

bool DrinkData::writeChanges(void) {
    QFile dbFile;
    
    dbFile.setName(filename);
    if ( !dbFile.open( IO_WriteOnly | IO_Truncate )) {
        return false;
    }
    
    QTextStream t( &dbFile);
    
    DrinkList::Iterator it = items.begin();
    while ( it != items.end() ) {
        t << "# " << (*it).getName() << '\n';
        t << (*it).getIngredients() << '\n';
        ++it;
    }
    
    dbFile.close();
    
    return true;
}
    

// ----- DrinkItem -----

DrinkItem::DrinkItem(void) {
}

DrinkItem::DrinkItem(const QString &name, const QString &ingredients) {
    this->name = name;
    this->ingredients = ingredients;
}

const QString &DrinkItem::getName(void) {
    return name;
}

const QString &DrinkItem::getIngredients(void) {
    return ingredients;
}

void DrinkItem::setName(const QString &name) {
    this->name = name;
}

void DrinkItem::setIngredients(const QString &ingredients) {
    this->ingredients = ingredients;
}
