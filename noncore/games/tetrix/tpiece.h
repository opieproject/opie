/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/


#ifndef TPIECE_H
#define TPIECE_H

class TetrixPiece
{
public:
    TetrixPiece()                        {setRandomType();}
    TetrixPiece(int type)                {initialize(type % 7 + 1);}

    void setRandomType()                 {initialize(randomValue(7) + 1);}

    void rotateLeft();
    void rotateRight();

    int  getType()                       {return pieceType;}
    int  getXCoord(int index)            {return coordinates[index][0];}
    int  getYCoord(int index)            {return coordinates[index][1];}
    void getCoord(int index,int &x,int&y){x = coordinates[index][0];
                                          y = coordinates[index][1];}
    int  getMinX();
    int  getMaxX();
    int  getMinY();
    int  getMaxY();

    static void   setRandomSeed(double seed);
    static int    randomValue(int maxPlusOne);

private:
    void setXCoord(int index,int value)  {coordinates[index][0] = value;}
    void setYCoord(int index,int value)  {coordinates[index][1] = value;}
    void setCoords(int index,int x,int y){coordinates[index][0] = x;
                                          coordinates[index][1] = y;}
    void initialize(int type);

    int  pieceType;
    int  coordinates[4][2];

    static double randomSeed;
};

#endif
