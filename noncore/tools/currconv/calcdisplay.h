/****************************************************************************
 *
 * File:        calcdisplay.h
 *
 * Description: Header file for the class LCDDisplay
 *
 *
 * Authors:     Eric Santonacci <Eric.Santonacci@talc.fr>
 *
 * Requirements:    Qt
 *
 ***************************************************************************/
#ifndef CALCDISPLAY_H
#define CALCDISPLAY_H

#include "currency.h"

#include <qhbox.h>
#include <qlcdnumber.h>
#include <qhgroupbox.h>
#include <qpushbutton.h>
#include <qcombobox.h>

namespace Opie {
    namespace Core {
        class OProcess;
    }
}

class LCDDisplay : public QHBox{

Q_OBJECT
public:
    LCDDisplay( QWidget *parent=0, const char *name=0 );
    virtual ~LCDDisplay();

public slots:
    void setValue(double);
    void swapLCD(void);
    void cbbxChange(void);
    void dataRetrieved(Opie::Core::OProcess*);
    void slotUpdate();
    void slotSettings();
    void slotCheckData();
    void slotDataInfo();

signals:
    void keyPressed(int key);
    void keyReleased(int key);

protected:
    bool eventFilter( QObject *obj, QEvent *ev );

private:
    int         grpbxStyle;

    QHGroupBox  *grpbxTop;
    QComboBox   *cbbxTop;
    QLCDNumber  *lcdTop;

    QHGroupBox  *grpbxBottom;
    QComboBox   *cbbxBottom;
    QLCDNumber  *lcdBottom;

    QPushButton *btnSwap;

    int iCurrentLCD; // 0=top, 1=bottom

    CurrencyConverter m_curr;
    int m_updateMode;
    QString m_dataDir;
    QString m_lastRate1, m_lastRate2;
    QStringList m_visibleRates;

    void readConfig();
    void saveConfig();
    void reloadRateData();
    void refreshRates();
    void updateRateCombo( QComboBox *combo, const QStringList &lst, const QString &dfl );
    void startDownload(const QString remoteFile, const QString localFile, int updateMode);
};

#endif // CALCDISPLAY_H
