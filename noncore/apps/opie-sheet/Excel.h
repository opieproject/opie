/*
               =.            This file is part of the Opie Project
             .=l.            Copyright (C) 2004 Opie Developer Team <opie-devel@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

/* QT */
#include <qstring.h>
#include <qarray.h>
#include <qlist.h>

/* STD */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/types.h>
#include <strings.h>

#define DATEFORMAT 0x1
#define NUMBERFORMAT 0x2

#define BIFF8 0x600
#define BIFF7 0x500
#define WBKGLOBAL 0x5
#define WRKSHEET 0x10

#define XL_ARRAY 0x221
#define XL_BOUNDSHEET 0x85
#define XL_BOF 0x809
#define XL_BOOLERR 0x205
#define XL_CONTINUE 0x3c
#define XL_DIMENSION 0x200
#define XL_EOF 0x0a
#define XL_EXTSST 0xff
#define XL_FORMULA 0x406
#define XL_FORMULA2 0x6
#define XL_FORMAT 0x41e
#define XL_INDEX 0x20b
#define XL_LABEL 0x204
#define XL_LABELSST 0xfd
#define XL_MULRK  0xbd
#define XL_NAME 0x18
#define XL_NOTE 0x1c
#define XL_NUMBER 0x203
#define XL_RK  0x7e
#define XL_RK2  0x27e
#define XL_ROW 0x208
#define XL_SST 0xfc
#define XL_STRING 0x207
#define XL_TXO 0x1b6
#define XL_XF 0xe0
#define XL_UNKNOWN 0xffff

#define CELL_LABEL 0x2
#define CELL_NUMBER 0x3
#define CELL_DATE 0x4
#define CELL_BOOLEAN 0x5
#define CELL_ERROR 0x6



class ExcelFormat
{
public:
    int code;
    int type;
    QString format;
    ExcelFormat();
    ExcelFormat(int c,int t, QString s);
};

struct xfrecord
{
    int code;
    int type;
    QString format;
};

class ExcelCell
{
public:
    int type;
    int row,col;
    int xfindex; //xf format index of cell
    int valuei;
    double valued;
    QString valuec;

};

class ExcelBREC
{
public:
    int code;
    int length;
    int position;
    char* data;
};

class SSTList
{
public:
    QArray <ExcelBREC*> rec;
};

class ExcelSheet
{
public:
    QString name;
    ExcelBREC BOFRecord;
    int position;
    int type;
    int rows;
    int cols;

    int cellsize,rowalloc,cellalloc;
    QArray <ExcelCell*> Cells;
    bool InitCells(void); // true if ok
    ExcelCell* Get(int row, int col);
    void Set(int row, int col, ExcelCell* cell);

};

struct mulrk
{
    int row;
    int first;
    int last;
    int numrks;
    QArray<int> rknumbers;
    QArray<double> rkdbls;
    QArray<int> xfindices;
};

class ExcelBook
{
public:
    FILE *File;
    int Position;
    //int stringcount;
    QArray <QString*> SharedStrings;
    //int xfcount;
    QArray <ExcelFormat*> XFRecords;
    //int Sheetcount;
    QArray <ExcelSheet*> Sheets;
    //int name count;
    QArray <QString*> Names;

    QString dateformat;
    int Version;
    int endian;
    int Integer2Byte(int b1, int b2 );
    int Integer4Byte(int b1, int b2, int b3, int b4 );
    int Integer2ByteFile(FILE *f);
    float Float4Byte(int b1, int b2, int b3, int b4);
    double Double4Byte(int b1, int b2, int b3, int b4);
    double Double8Byte(int b1, int b2, int b3, int b4, int b5, int b6, int b7, int b8);
    void DetectEndian(void);

    bool OpenFile(char *Filename); // true if ok
    bool CloseFile(void); // true if ok
    void SeekPosition(int pos); // go to Pos
    void SeekSkip(int pos); // skips pos bytes.
    int FileEOF(void); //returns -1 if EOF else 0
    int Get2Bytes(void); //gets an int from the file
    char* Read(int pos, int length);
    QString ReadUnicodeChar(int pos, int length);
    QString* GetString(int num); //gets the num string from SharedStrings;
    int SeekBOF(void);
    ExcelBREC* GetBREC(void);
    ExcelBREC* PeekBREC(void);
    char* GetDataOfBREC(ExcelBREC* record);
    void ConvertCharToArray(ExcelBREC* record, char* chars, int length);
    int SheetHandleRecord(ExcelSheet* sheet, ExcelBREC* record);
    int ReadSheet(ExcelSheet* sheet); //read the sheet sheet*
    ExcelSheet* GetSheet(void);
    void ParseSheets(void);
    void GetSheets(void);

    bool ParseBook(char *file); // THIS IS THE MAIN PARSE FUNCTION of file
    QString GetASCII(char* inbytes, int pos, int chars);
    QString GetUnicode(char * inbytes, int pos, int chars);
    void HandleBoundSheet( ExcelBREC* rec);
    void HandleName(ExcelSheet* sheet, ExcelBREC* rec);
    ExcelFormat* GetFormatting(int xf);
    void HandleSetOfSST(ExcelBREC* rec/*, SSTList* cont*/, char* bytes);
    char* MergeBytesFromSSTs(ExcelBREC* rec,SSTList* cont);
    void HandleSST(ExcelBREC* rec);
    void HandleLabelSST(ExcelSheet* sheet, ExcelBREC* rec);
    ExcelCell* CellLabel(int row, int col, QString str);
    ExcelCell* CellNumber(int row, int col, int index, double d);
    QString* CellDataString(ExcelSheet* sh, int row, int col);
    int CellGetPrecision(double d);
    void CellSetDateFormat(char *d);
    void HandleMulrk(ExcelSheet* sheet, ExcelBREC* record);
    void MulrkRead(struct mulrk *mulrk, char* data);
    void HandleNumber(ExcelSheet* sheet, ExcelBREC* record);
    void HandleFormat(ExcelBREC* rec);
    void HandleXF(ExcelBREC* rec);
    void HandleRK(ExcelSheet* sheet, ExcelBREC* record);
    void HandleFormula(ExcelSheet* sheet, ExcelBREC* record);
    QString GetFormula(int row, int col, ExcelSheet* sheet, char* data, int sz);
    QString FindCellName(int row, int col);
};

