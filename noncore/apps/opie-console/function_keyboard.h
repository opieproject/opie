#ifndef OPIE_FUNCTION_KEYBOARD_H
#define OPIE_FUNCTION_KEYBOARD_H

#include <qpe/config.h>
#include <qframe.h>
#include <qpainter.h>
#include <qvbox.h>
#include <qgroupbox.h>
#include <qmap.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include "profiledialogwidget.h"


class FKey {

public:

    FKey(): qcode(0), unicode(0) {};
    FKey(const QString &l, ushort q, ushort u): label(l), qcode(q), unicode(u) {};

    QString getL() { return label; }
    ushort getQ() { return qcode; }
    ushort getU() { return unicode; }


private:

    QString label;
    ushort qcode;
    ushort unicode;
};

class FunctionKeyboard : public QFrame {
    Q_OBJECT

public:
    FunctionKeyboard(QWidget *parent = 0);
    ~FunctionKeyboard();

    void changeRows(int);
    void changeCols(int);

    void paintEvent(QPaintEvent *);
    void paintKey(int, int);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void resizeEvent(QResizeEvent*);
    QSize sizeHint() const;

signals:

    void keyPressed(ushort, ushort, bool, bool, bool, ushort, ushort);

private:

    void loadDefaults();


private:

    // thie key for the map is the row/col
    QMap<QString, FKey> keys;

    uint numRows;
    int test;
    uint numCols;
    uint keyHeight;
    double keyWidth; // decimal point matters!

    int pressedRow, pressedCol;

    QObject *parent;

};

class FunctionKeyboardConfig : public ProfileDialogKeyWidget {

    Q_OBJECT

public:
    FunctionKeyboardConfig(const QString& name, QWidget *wid, const char* name = 0l );
    ~FunctionKeyboardConfig();

    void load(const Profile&);
    void save(Profile&);

private slots:

    void slotKeyPressed(ushort, ushort, bool, bool, bool, ushort, ushort);
    void slotChangeRows(int);
    void slotChangeCols(int);
    void slotChangeIcon(int);

private:

    FunctionKeyboard *kb;
    QSpinBox *m_rowBox, *m_colBox;
    QComboBox *m_labels;

};

#endif
