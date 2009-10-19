#include "datebookweeklstdayhdr.h"

#include "namespace_hack.h"

DateBookWeekLstDayHdr::DateBookWeekLstDayHdr(const QDate &d, bool /* onM */,
                         QWidget* parent,
                         const char* name,
                         WFlags fl )
    : DateBookWeekLstDayHdrBase(parent, name, fl) {

    date=d;

    static const QString wdays=tr("MTWTFSSM",  "Week days");
    char day=wdays[d.dayOfWeek()-1];

    //dont use dayOfWeek() to save space !
    label->setText( QString(QString(QChar(day))) + " " +QString::number(d.day()) );

     add->setText("+");

    if (d == QDate::currentDate()) {
        QPalette pal=label->palette();
        pal.setColor(QColorGroup::Foreground, QColor(0,0,255));
        label->setPalette(pal);

        /*
        QFont f=label->font();
        f.setItalic(true);
        label->setFont(f);
        label->setPalette(QPalette(QColor(0,0,255),label->backgroundColor()));
        */
    } else if (d.dayOfWeek() == 7) { // FIXME: Match any holiday
        QPalette pal=label->palette();
        pal.setColor(QColorGroup::Foreground, QColor(255,0,0));
        label->setPalette(pal);
    }

    connect (label, SIGNAL(clicked()), this, SLOT(showDay()));
    connect (add, SIGNAL(clicked()), this, SLOT(newEvent()));
}

void DateBookWeekLstDayHdr::showDay() {
    emit showDate(date.year(), date.month(), date.day());
}

void DateBookWeekLstDayHdr::newEvent() {
    QDateTime start, stop;
    start=stop=date;
    start.setTime(QTime(10,0));
    stop.setTime(QTime(12,0));

    emit addEvent(start,stop);
}
