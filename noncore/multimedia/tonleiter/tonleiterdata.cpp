#include "tonleiterdata.h"

TonleiterData::TonleiterData(QObject* parent)
:QObject(parent)
{
    drawnames=true;
    loadData();
}
//****************************************************************************
TonleiterData::~TonleiterData()
{
    saveData();
}
//****************************************************************************
void TonleiterData::setCurrentInstrumetID(int id)
{
    if(id>=0 && id<noOfInstruments())
    {
        currentInstrumentID=id;
        emit dataChange();
    }
}
//****************************************************************************
void TonleiterData::setCurrentScaleID(int id)
{
    if(id>=0 && id<noOfScales())
    {
        currentScaleID=id;
        emit dataChange();
    }
}
//****************************************************************************
void TonleiterData::setCurrentNote(int id)
{
    if(id>=0 && id<12)
    {
        currentNote=id;
        emit dataChange();
    }
}
//****************************************************************************
void TonleiterData::setDrawNames(bool dn)
{
    drawnames=dn;
    emit dataChange();
}
//****************************************************************************
void TonleiterData::loadData()
{
    QValueList<int> strings1;
    strings1.append(Note::getNoteFromName("B",0));
    strings1.append(Note::getNoteFromName("E",1));
    strings1.append(Note::getNoteFromName("A",1));
    strings1.append(Note::getNoteFromName("D",2));
    strings1.append(Note::getNoteFromName("G",2));
    strings1.append(Note::getNoteFromName("C",3));
    instruments.append(Instrument("Thumb6",26,strings1));

    QValueList<int> strings2;
    strings2.append(Note::getNoteFromName("E",1));
    strings2.append(Note::getNoteFromName("A",1));
    strings2.append(Note::getNoteFromName("D",2));
    strings2.append(Note::getNoteFromName("G",2));
    instruments.append(Instrument("Bass 4",12,strings2));

    currentInstrumentID=1;

    QValueList<int> scale1;
    scale1.append(0);
    scale1.append(2);
    scale1.append(3);
    scale1.append(5);
    scale1.append(7);
    scale1.append(8);
    scale1.append(10);
    scales.append(Scale("Minor",scale1));

    QValueList<int> scale2;
    scale2.append(0);
    scale2.append(2);
    scale2.append(4);
    scale2.append(5);
    scale2.append(7);
    scale2.append(9);
    scale2.append(10);
    scales.append(Scale("Major",scale2));

    currentScaleID=0;

    currentNote=Note::getNoteFromName("A",0);
}
//****************************************************************************
void TonleiterData::saveData()
{
}
//****************************************************************************
int TonleiterData::noOfInstruments()
{
    return (int)instruments.count();
}
//****************************************************************************
Instrument TonleiterData::getInstrument(int id)
{
    if(id>=0 && id<noOfInstruments())
        return instruments[id];
    else
    {
        qDebug("undefined insrtument %d of %d",id,instruments.count());
        return Instrument();
    }

}
//****************************************************************************
int TonleiterData::getCurrentInstrumentID()
{
    return currentInstrumentID;
}
//****************************************************************************
int TonleiterData::noOfScales()
{
    return (int)scales.count();
}
//****************************************************************************
Scale TonleiterData::getScale(int id)
{
    if(id>=0 && id<noOfScales())
        return scales[id];
    else
    {
        qDebug("Undefined scale");
        return Scale();
    }
}
//****************************************************************************
int TonleiterData::getCurrentScaleID()
{
    return currentScaleID;
}
//****************************************************************************
int TonleiterData::getCurrentBaseNote()
{
    return currentNote;
}
//****************************************************************************
bool TonleiterData::isDrawNames()
{
    return drawnames;
}
//****************************************************************************
//****************************************************************************

