#include "tonleiterdata.h"

#include <qpe/config.h>

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
/*
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
*/
    Config load("Tonleiter");
    load.setGroup("General");
    currentNote=load.readNumEntry("currentNote");
    drawnames=load.readBoolEntry("drawnames");
    currentInstrumentID=load.readNumEntry("currentInstrumentID");
    currentScaleID=load.readNumEntry("currentScaleID");

    load.setGroup("Instrument");
    int noOfInstr=load.readNumEntry("noOfInstruments",0);
    for(int i=0;i<noOfInstr;i++)
    {
        QStringList instlist=load.readListEntry("Inst"+QString::number(i),';');
        QValueList<int> strings;
        for(int st=2;st<(int)instlist.count();st++)
            strings.append(instlist[st].toInt());
        instruments.append(Instrument(instlist[0],instlist[1].toInt(),strings));
    }

    load.setGroup("Scale");
    int scaleno=load.readNumEntry("noOfScales",0);
    for(int s=0;s<scaleno;s++)
    {
        QStringList scalelist=load.readListEntry("Scale"+QString::number(s),';');
        QValueList<int> halftones;
        for(int ht=1;ht<(int)scalelist.count();ht++)
            halftones.append(scalelist[ht].toInt());
        scales.append(Scale(scalelist[0],halftones));
    }

}
//****************************************************************************
void TonleiterData::saveData()
{
    Config save("Tonleiter");
    save.setGroup("General");
    save.writeEntry("currentNote",currentNote);
    save.writeEntry("drawnames",drawnames);
    save.writeEntry("currentInstrumentID",currentInstrumentID);
    save.writeEntry("currentScaleID",currentScaleID);

    save.setGroup("Instrument");
    save.writeEntry("noOfInstruments",noOfInstruments());
    for(int i=0;i<noOfInstruments();i++)
    {
        QStringList instlist;
        Instrument inst=getInstrument(i);
        instlist.append(inst.instName());
        instlist.append(QString::number(inst.noOfFrets()));
        for(int st=0;st<inst.noOfStrings();st++)
            instlist.append(QString::number(inst.string(st)));
        save.writeEntry("Inst"+QString::number(i),instlist,';');
    }

    save.setGroup("Scale");
    save.writeEntry("noOfScales",noOfScales());
    for(int s=0;s<noOfScales();s++)
    {
        QStringList scalelist;
        Scale scale=getScale(s);
        scalelist.append(scale.scaleName());
        for(int ht=0;ht<scale.noOfHaltones();ht++)
            scalelist.append(QString::number(scale.getHalfTone(ht)));
        save.writeEntry("Scale"+QString::number(s),scalelist,';');
    }
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

