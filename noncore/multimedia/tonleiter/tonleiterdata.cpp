#include "tonleiterdata.h"

#include <qpe/config.h>

using namespace Data;


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
    Config load("Tonleiter");
    if(!load.isValid())
    {
        //write some default values
        load.setGroup("General");
        load.writeEntry("currentInstrumentID",1);
        load.writeEntry("currentNote",0);
        load.writeEntry("currentScaleID",1);
        load.writeEntry("drawnames",1);
        load.setGroup("Instrument");
        load.writeEntry("Inst0","Thumb6;26;11;16;21;26;31;36;");
        load.writeEntry("Inst1","Bass 4;12;16;21;26;31;");
        load.writeEntry("noOfInstruments",2);
        load.setGroup("Scale");
        load.writeEntry("Scale0","ionian;0;2;4;5;7;9;11;");
        load.writeEntry("Scale1","aeolian;0;2;3;5;7;8;10;");
        load.writeEntry("Scale2","dorian;0;2;3;5;7;9;10;");
        load.writeEntry("Scale3","phrygian;0;1;3;5;7;8;10;");
        load.writeEntry("Scale4","lydian;0;2;4;6;7;9;11;");
        load.writeEntry("Scale5","mixolyd.;0;2;4;5;7;9;10;");
        load.writeEntry("noOfScales",6);

    }

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

