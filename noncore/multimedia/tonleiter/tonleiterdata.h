#ifndef TONLEITER_DATA_H
#define TONLEITER_DATA_H

#include <qobject.h>

#include "tonleiterdatahelper.h"

class TonleiterData : public QObject
{
    Q_OBJECT
private:
    QValueList<Instrument> instruments;
    int currentInstrumentID;

    QValueList<Scale> scales;
    int currentScaleID;

    int currentNote;
    bool drawnames;
public:
    TonleiterData(QObject* parent);
    ~TonleiterData();
signals:
    void dataChange();
public slots:
    void setCurrentInstrumetID(int id);
    void setCurrentScaleID(int id);
    void setCurrentNote(int id);
    void setDrawNames(bool dn);
private:
    void loadData();
    void saveData();
public:
    int noOfInstruments();
    Instrument getInstrument(int id);
    int getCurrentInstrumentID();

    int noOfScales();
    Scale getScale(int id);
    int getCurrentScaleID();

    int getCurrentBaseNote();
    bool isDrawNames();
};

#endif //TONLEITER_DATA_H
