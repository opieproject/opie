#ifndef TONLEITER_DATA_HELPER_H
#define TONLEITER_DATA_HELPER_H

#include <qstring.h>
#include <qvaluelist.h>

namespace Data
{
    namespace Note
    {
        const QString notenames[]={"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};

        int getOctaveOfNote(int note);
        QString getNameOfNote(int note);
        int getNoteFromName(QString name,int octave);
        int octaveOfBaseNote(int base,int note);
    };

    class Instrument
    {
    private:
        QString name;
        int frets;
        QValueList<int> strings;
    public:
        Instrument();
        Instrument(QString name,int frets,QValueList<int> strings);
        ~Instrument();
    public:
        int noOfStrings();
        int noOfFrets();
        QString instName();
        int string(int id);
        int noOfOctaves();
    };

    class Scale
    {
    private:
        QValueList<int> halftones;
        QString name;
    public:
        Scale();
        Scale(QString name,QValueList<int> halftones);
        ~Scale();
    public:
        int noOfHaltones();
        int getHalfTone(int id);
        QString scaleName();
        bool noteInScale(int base,int note);
    };
};

#endif //TONLEITER_DATA_HELPER_H

