#ifndef OPIE_DATEBOOK_MONTHVIEW_H
#define OPIE_DATEBOOK_MONTHVIEW_H

#include <qpixmap.h>
#include <qdatetime.h>
#include <qwidget.h>
#include <qvbox.h>
#include <qlabel.h>

#include <opie2/opimevent.h>
#include <opie2/opimoccurrence.h>

#include "view.h"

class Config;
class ODateBookMonth;

namespace Opie {
namespace Datebook {
    class MainWindow;
    class MonthView : public View {
        Q_OBJECT
    public:
        MonthView( MainWindow* window,  QWidget* parent );

        virtual QPixmap pixmap() const;
        virtual QCString type() const;
        virtual QString name() const;
        virtual QString description() const;
        virtual const OPimOccurrence *currentItem() const;
        virtual QDate date() const;

        virtual void currentRange( const QDateTime& src, const QDateTime& from);

        virtual void clockChanged();
        virtual void dayChanged();
        virtual void timeChanged();
        virtual void showDay( const QDate& date );
        virtual QWidget* widget();
        virtual void reschedule();

        void setDate( const QDate &d );

    protected:
        ODateBookMonth *m_widget;

        virtual void doLoadConfig( Config* );
        virtual void doSaveConfig( Config* );

        void initUI(QWidget *parent);
        void getEvents();

    protected slots:
        void slotPopulateEvents();
        void dateClicked(int,int,int);
    };
}
}

#endif
