#include "tonleiterdatahelper.h"

int Note::getOctaveOfNote(int note)
{
    int remain=note%12;
    return (note-remain)/12;
}
//****************************************************************************
QString Note::getNameOfNote(int note)
{
    int octave=getOctaveOfNote(note);
    return notenames[note-12*octave];
}
//****************************************************************************
int Note::getNoteFromName(QString name,int octave)
{
    int notevalue=0;
    for(int a=0;a<12;a++)
    {
        if(name==notenames[a])
        {
            notevalue=a;
            break;
        }
    }
    return notevalue+12*octave;
}
//****************************************************************************
int Note::octaveOfBaseNote(int base,int note)
{
    int normnote = (note>=base) ? note-base : (12-base)+note;
    int octave=getOctaveOfNote(normnote);
    //qDebug("note %d of %d base is norm %d -> ocatve %d",note,base,normnote,octave);
    return octave;
}
//****************************************************************************
//****************************************************************************
Instrument::Instrument()
{
    name="UNDEFINED";
    frets=0;
}
//****************************************************************************
Instrument::Instrument(QString name,int frets,QValueList<int> strings)
:name(name),frets(frets),strings(strings)
{
}
//****************************************************************************
Instrument::~Instrument()
{
}
//****************************************************************************
int Instrument::noOfStrings()
{
    return (int)strings.count();
}
//****************************************************************************
int Instrument::noOfFrets()
{
    return frets;
}
//****************************************************************************
QString Instrument::instName()
{
    return name;
}
//****************************************************************************
int Instrument::string(int id)
{
    return strings[id];
}
//****************************************************************************
int Instrument::noOfOctaves()
{
    int lowest=strings[0];
    int highest=strings[strings.count()-1]+frets;
    return (int)((highest-lowest)/12.0);
}
//****************************************************************************
//****************************************************************************
Scale::Scale()
{
    name="UNDEFINED";
}
//****************************************************************************
Scale::Scale(QString name,QValueList<int> halftones)
:name(name),halftones(halftones)
{
}
//****************************************************************************
Scale::~Scale()
{
}
//****************************************************************************
int Scale::noOfHaltones()
{
    return (int)halftones.count();
}
//****************************************************************************
int Scale::getHalfTone(int id)
{
    if(id>=0 && noOfHaltones()<id)
    {
        return halftones[id];
    }
    else
        return 0;
}
//****************************************************************************
QString Scale::scaleName()
{
    return name;
}
//****************************************************************************
bool Scale::noteInScale(int base,int note)
{
    int octave=Note::getOctaveOfNote(note);
    note-=12*octave;
    int normnote = (note>=base) ? note-base : (12-base)+note;

    if(halftones.contains(normnote)>0)
    {
        //qDebug("OK : base : %d, note %d -> norm %d",base,note,normnote);
        return true;
    }
    else
    {
        //qDebug("BAD : base : %d, note %d -> norm %d",base,note,normnote);
        return false;
    }
}
//****************************************************************************
//****************************************************************************
