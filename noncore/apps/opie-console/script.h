#ifndef CONSOLE_SCRIPT_H
#define CONSOLE_SCRIPT_H

#include <qstring.h>

/* Very simple scripting - this class stores keys received
 * by emulation_layer */

class Script {
public:
    /* Construct an empty script */
    Script();
    
    /* Load a script from a text file */
    Script(const QString fileName);

    /* Append a line to the script */
    void appendString(const QString string);

    /* Save this script to a file */
    void saveTo(const QString fileName) const;

    /* Return the script's content */
    QString script() const;
protected:
    QString m_script;
};


#endif /* CONSOLE_SCRIPT_H */
