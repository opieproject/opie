#ifndef _ABVIEW_H_
#define _ABVIEW_H_

#include <qwidget.h>
#include <qwidgetstack.h> 

#include <qpe/categories.h>
#include <opie/ocontact.h>
#include <opie/ocontactaccess.h>

#include "contacteditor.h"
#include "abtable.h"
#include "ablabel.h"

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
    void setShowByCategory( const QString& cat );
    void setShowToView( Views view );
    void setShowByLetter( char c );
    void setListOrder( const QValueList<int>& ordered );

    // Add Entry and put to current
    void addEntry( const OContact &newContact );
    void removeEntry( const int UID );
    void replaceEntry( const OContact &contact );
    OContact currentEntry();

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
    void updateView(); 
    void clearForCategory();
    bool contactCompare( const OContact &cnt, int category );
    void parseName( const QString& name, QString *first, QString *middle,
		    QString * last );

    Categories mCat;
    bool m_inSearch;
    bool m_inPersonal;
    int m_curr_category;
    Views m_curr_View;
    Views m_prev_View;
    int m_curr_Contact;
    
    OContactAccess* m_contactdb;
    OContactAccess* m_storedDB;
    OContactAccess::List m_list;

    QWidgetStack* m_viewStack;
    AbTable* m_abTable;
    AbLabel* m_ablabel;

    QValueList<int> m_orderedFields;
};


#endif
