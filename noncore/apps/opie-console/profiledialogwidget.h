#ifndef OPIE_PROFILE_DIALOG_WIDGET_H
#define OPIE_PROFILE_DIALOG_WIDGET_H

#include <qwidget.h>

#include "profile.h"

class ProfileDialogWidget : public QWidget {
    Q_OBJECT
public:
    enum Type {
        Connection,
        Terminal
    };
    ProfileDialogWidget( const QString& name, QWidget* parent, const char* name = 0l);
    ~ProfileDialogWidget();

    /*
     * load data from a Profile into
     * the DialogWidget
     */
    virtual void load( const Profile& ) = 0;

    /*
     * save data into a profile
     * from the DialogWidget
     */
    virtual void save( Profile& ) = 0;

    virtual Type type()const = 0;
};
class ProfileDialogTerminalWidget : public ProfileDialogWidget {
    Q_OBJECT
public:
    ProfileDialogTerminalWidget( const QString& name, QWidget* widget,
                                 const char* name  =0l);
    ~ProfileDialogTerminalWidget();
    Type type()const;
};
class ProfileDialogConnectionWidget : public ProfileDialogWidget {
    Q_OBJECT
public:
    ProfileDialogConnectionWidget( const QString& name, QWidget* parent,
                                   const char* name =0l);
    ~ProfileDialogConnectionWidget();
    Type type() const;

};

#endif
