#ifndef _ABCONFIG_H_
#define _ABCONFIG_H_

#include <qstringlist.h> 

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
    
    void setUseRegExp( bool v );
    void setUseWildCards( bool v );
    void setBeCaseSensitive( bool v ); 
    void setUseQtMail( bool v );
    void setUseOpieMail( bool v );
    void setFontSize( int v );

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

    bool m_changed;
};


#endif
