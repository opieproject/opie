#ifndef _ABVIEW_H_
#define _ABVIEW_H_

#include <opie2/opimcontact.h>
#include <opie2/ocontactaccess.h>

#include <qpe/categories.h>

#include <qwidget.h>
#include <qwidgetstack.h>

#include "contacteditor.h"
#include "abtable.h"
#include "ablabel.h"
#include "abconfig.h"

class AbView: public QWidget
{
    Q_OBJECT

public:
    enum Views{ TableView=0, CardView, PersonalView };

    AbView( QWidget* parent, const QValueList<int>& ordered );
    ~AbView();

    bool save();
    void load();
    void reload();
    void clear();

    void setView( Views view );
    void showPersonal( bool personal );
    void setCurrentUid( int uid );
    void setShowByCategory( const QString& cat );
    void setShowToView( Views view );
    void setShowByLetter( char c, AbConfig::LPSearchMode mode = AbConfig::LastName );
    void setListOrder( const QValueList<int>& ordered );

    // Add Entry and put to current
    void addEntry( const Opie::OPimContact &newContact );
    void removeEntry( const int UID );
    void replaceEntry( const Opie::OPimContact &contact );
    Opie::OPimContact currentEntry();

    void inSearch()  { m_inSearch = true; }
    void offSearch();

    QString showCategory() const;
    QStringList categories();

signals:
	void signalNotFound();
	void signalClearLetterPicker();
	void signalViewSwitched ( int );

public slots:
    void slotDoFind( const QString &str, bool caseSensitive, bool useRegExp,
		     bool backwards, QString category = QString::null );
    void slotSwitch();

private:
    void updateListinViews();
    void updateView( bool newdata = false );
    void clearForCategory();
    bool contactCompare( const Opie::OPimContact &cnt, int category );
    void parseName( const QString& name, QString *first, QString *middle,
		    QString * last );

    Categories mCat;
    bool m_inSearch;
    bool m_inPersonal;
    int m_curr_category;
    Views m_curr_View;
    Views m_prev_View;
    int m_curr_Contact;

    Opie::OPimContactAccess* m_contactdb;
    Opie::OPimContactAccess* m_storedDB;
    Opie::OPimContactAccess::List m_list;

    QWidgetStack* m_viewStack;
    AbTable* m_abTable;
    AbLabel* m_ablabel;

    QValueList<int> m_orderedFields;
};


#endif
