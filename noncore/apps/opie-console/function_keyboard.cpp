#include "function_keyboard.h"

#include <qpe/resource.h>
#include <qpe/qpeapplication.h>
#include <qsizepolicy.h>
#include <qevent.h>
#include <qwindowsystem_qws.h>
#include <qapplication.h>
#include <qlayout.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qdir.h>

/* FunctionKeyboard {{{1 */

FunctionKeyboard::FunctionKeyboard(QWidget *parent) :
    QFrame(parent), numRows(2), numCols(11),
    pressedRow(0), pressedCol(0) {

    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed));

    Config conf("opie-console-keys");
    conf.setGroup("keys");
    for (uint r = 0; r < numRows; r++)
        for (uint c = 0; c < numCols; c++) {

            QString handle = "r" + QString::number(r) + "c" + QString::number(c);
            QStringList value_list = conf.readListEntry( handle, '|');

            if (value_list.isEmpty()) continue;

            keys.insert(

                 handle,
                 FKey (value_list[0], value_list[1].toUShort(), value_list[2].toUShort())
            );
        }
    //qWarning("loaded %d keys", keys.count());
    if (keys.isEmpty()) loadDefaults();



}

FunctionKeyboard::~FunctionKeyboard() {

}

void FunctionKeyboard::changeRows(int r) {

    numRows = r;
    repaint(false);
}
void FunctionKeyboard::changeCols(int c) {

    numCols = c;
    keyWidth = (double)width()/numCols; // have to reset this thing too
    repaint(false);
}

void FunctionKeyboard::paintEvent(QPaintEvent *e) {

    QPainter p(this);
    p.setClipRect(e->rect());
    p.fillRect(0, 0, width(), height(), QColor(255,255,255));

    p.setPen(QColor(0,0,0));

    /* those decimals do count! becomes short if use plain int */
    for (double i = 0; i <= width(); i += keyWidth) {

        p.drawLine((int)i, 0, (int)i, height());
    }

    // sometimes the last line doesnt get drawn
    p.drawLine(width() -1, 0, width() -1, height());

    for (int i = 0; i <= height(); i += keyHeight) {

        p.drawLine(0, i, width(), i);
    }

    for (uint r = 0; r < numRows; r++) {
        for (uint c = 0; c < numCols; c++) {

            QString handle = "r" + QString::number(r) + "c" + QString::number(c);
            if (keys.contains(handle)) {

                p.drawText(
                                c * keyWidth + 1, r * keyHeight + 1,
                                keyWidth, keyHeight,
                                Qt::AlignHCenter | Qt::AlignVCenter,
                                keys[handle].getL()
                );
            }
        }
    }
}

void FunctionKeyboard::paintKey(int row, int col) {

    QPainter p(this);

    p.fillRect(QRect(QPoint(col * keyWidth + 1, row * keyHeight + 1),
                     QPoint((col + 1) * keyWidth - 1, row * keyHeight + keyHeight- 1)),
               (pressedRow != -1 && pressedCol != -1 ) ? QColor(97,119,155) : QColor(255,255,255));
    p.drawText(
                    col * keyWidth + 1, row * keyHeight + 1,
                    keyWidth, keyHeight,
                    Qt::AlignHCenter | Qt::AlignVCenter,
                    keys["r" + QString::number(row) + "c" + QString::number(col)].getL()
    );

    if (row == numRows) {

        // sometimes it doesnt draw the last line
        p.drawLine((col+1) * keyWidth -2, row * keyHeight,
                   (col+1) * keyWidth -2, (row + 1) * keyHeight
        );
    }

}

void FunctionKeyboard::mousePressEvent(QMouseEvent *e) {

    pressedRow = e->y() / keyHeight;
    pressedCol = (int) (e->x() / keyWidth);

    paintKey(pressedRow, pressedCol);

    // emit that sucker!
    FKey k = keys["r" + QString::number(pressedRow) + "c" + QString::number(pressedCol)];
    emit keyPressed(k.getU(), k.getQ(), 0, 1, 0, pressedRow, pressedCol);

}

void FunctionKeyboard::mouseReleaseEvent(QMouseEvent *) {

    if (pressedRow != -1 && pressedRow != -1) {

        int row = pressedRow; pressedRow = -1;
        int col = pressedCol; pressedCol = -1;
        paintKey(row, col);

        FKey k = keys["r" + QString::number(row) + "c" + QString::number(col)];
        emit keyPressed(k.getU(), k.getQ(), 0, 0, 0, pressedRow, pressedCol);
    }

}


void FunctionKeyboard::resizeEvent(QResizeEvent*) {

    /* set he default font height/width */
    QFontMetrics fm=fontMetrics();
    keyHeight = fm.lineSpacing() + 2;
    keyWidth = (double)width()/numCols;

}

QSize FunctionKeyboard::sizeHint() const {

    return QSize(width(), keyHeight * numRows + 1);
}

void FunctionKeyboard::loadDefaults() {

    /* what keys should be default? */
    keys.insert( "r0c0", FKey ("F1", 4144, 0));
    keys.insert( "r0c1", FKey ("F2", 4145, 0));
    keys.insert( "r0c2", FKey ("F3", 4145, 0));
    keys.insert( "r0c3", FKey ("F4", 4146, 0));
    keys.insert( "r0c4", FKey ("F5", 4147, 0));
    keys.insert( "r0c5", FKey ("F6", 4148, 0));
    keys.insert( "r0c6", FKey ("F7", 4149, 0));
    keys.insert( "r0c7", FKey ("F8", 4150, 0));
    keys.insert( "r0c8", FKey ("F9", 4151, 0));
    keys.insert( "r0c9", FKey ("F10", 4152, 0));
    keys.insert( "r0c10", FKey ("F11", 4153, 0));

    keys.insert( "r1c7", FKey ("Ho", 4112, 0));
    keys.insert( "r1c8", FKey ("End", 4113, 0));
    keys.insert( "r1c9", FKey ("PU", 4118, 0));
    keys.insert( "r1c10", FKey ("PD", 4119, 0));

}

/* FunctionKeyboardConfig {{{1 */

FunctionKeyboardConfig::FunctionKeyboardConfig(const QString& name, QWidget* parent, const char* na )
    : ProfileDialogKeyWidget(name, parent, na) {


    kb = new FunctionKeyboard(this);

    QGroupBox *dimentions = new QGroupBox(2, Qt::Horizontal, tr("Dimentions"), this);
    QLabel *l = new QLabel("Rows", dimentions);
    QSpinBox *m_rowBox = new QSpinBox(1, 15, 1, dimentions);
    connect (m_rowBox, SIGNAL(valueChanged(int)), this, SLOT(slotChangeRows(int)));
    l = new QLabel("Columns", dimentions);
    m_colBox = new QSpinBox(1, 15, 1, dimentions);
    connect (m_colBox, SIGNAL(valueChanged(int)), this, SLOT(slotChangeCols(int)));

    QGroupBox *editKey = new QGroupBox(2, Qt::Horizontal, tr("Edit Key"), this);
    l = new QLabel("Label", editKey);
    /*
    m_labels = new QComboBox(false, editKey);
    labels->insertItem("text");

    QStringList files = QDir(QPEApplication::qpeDir() + "pics/console/keys/", "*.png").entryList();

    for (uint i = 0; i < files.count(); i++) {

        m_labels->insertItem(Resource::loadPixmap("console/keys/" + files[i]));
    }
    connect (m_labels, SIGNAL(activated(int)), this, SLOT(slotChangeIcon(int)));
    */

    QVBoxLayout *root = new QVBoxLayout(this, 2);
    root->addWidget(kb);
    root->addWidget(dimentions);
    root->addWidget(editKey);
}
FunctionKeyboardConfig::~FunctionKeyboardConfig() {

}
void FunctionKeyboardConfig::load (const Profile& prof) {
    //int i = prof.readNumEntry("keb_rows", 1);
    m_rowBox->setValue( 2 );
}
void FunctionKeyboardConfig::save (Profile& prof) {

    prof.writeEntry("keb_rows", m_rowBox->value());

}
void FunctionKeyboardConfig::slotChangeRows(int r) {

    kb->changeRows(r);

    // have to do this so the whole thing gets redrawn
    kb->hide(); kb->show();
}
void FunctionKeyboardConfig::slotChangeCols(int c) {

    kb->changeCols(c);
}
void FunctionKeyboardConfig::slotKeyPressed(ushort, ushort, bool, bool, bool, ushort row, ushort col) {

}
void FunctionKeyboardConfig::slotChangeIcon(int index) {

    if (index == 0) {

        // is text
        //if(!labels->editable()) labels->setEditable(true);
    } else {

        // is a pixmap
        //if (labels->editable()) labels->setEditable(false);
    }
}
