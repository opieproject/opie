#ifndef _ABCONFIG_H_
#define _ABCONFIG_H_

#include <qstringlist.h> 
#include <qmainwindow.h>

class AbConfig
{
public:
    AbConfig();
    ~AbConfig();
    
    // Search Settings
    bool useRegExp() const;
    bool useWildCards() const;
    bool beCaseSensitive() const;
    bool useQtMail() const;
    bool useOpieMail() const;
    int  fontSize() const;
    QValueList<int> orderList() const;
    QMainWindow::ToolBarDock getToolBarPos() const;
    
    void setUseRegExp( bool v );
    void setUseWildCards( bool v );
    void setBeCaseSensitive( bool v ); 
    void setUseQtMail( bool v );
    void setUseOpieMail( bool v );
    void setFontSize( int v );
    void setOrderList( const QValueList<int>& list );
    void setToolBarDock( const QMainWindow::ToolBarDock v );

    void operator= ( const AbConfig& cnf );

    void load();
    void save();

protected:
/*     virtual void itemUp(); */
/*     virtual void itemDown(); */

    QStringList contFields;

    bool m_useQtMail;
    bool m_useOpieMail;
    bool m_useRegExp;
    bool m_beCaseSensitive; 
    int m_fontSize;
    QValueList<int> m_ordered;
    int m_barPos;

    bool m_changed;
};


#endif
