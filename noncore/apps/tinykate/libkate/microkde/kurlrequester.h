#ifndef MINIKDE_KURLREQUESTER_H
#define MINIKDE_KURLREQUESTER_H

class KLineEdit;

class KURLRequester : public QWidget
{
  public:
    KURLRequester( QWidget *parent );

    KLineEdit *lineEdit();

  private:
    KLineEdit *mLineEdit;
};

#endif
