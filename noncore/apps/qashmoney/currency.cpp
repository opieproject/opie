#include "currency.h"

Currency::Currency ( QWidget *parent ) : QWidget ( parent )
  {
    currencylabel = new QLabel ( "Currency", this );
    currencybox = new QComboBox ( this );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/AE.png"), "AE" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/AR.png"), "AR" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/AT.png"), "AT" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/AU.png"), "AU" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/BD.png"), "BD" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/BE.png"), "BE" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/BH.png"), "BH" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/BR.png"), "BR" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/BW.png"), "BW" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/CA.png"), "CA" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/CH.png"), "CH" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/CL.png"), "CL" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/CN.png"), "CN" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/CO.png"), "CO" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/CZ.png"), "CZ" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/DE.png"), "DE" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/DK.png"), "DK" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/DO.png"), "DO" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/EG.png"), "EG" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/ES.png"), "ES" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/EU.png"), "EU" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/FL.png"), "FL" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/FR.png"), "FR" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/GB.png"), "GB" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/GR.png"), "GR" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/HK.png"), "HK" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/HU.png"), "HU" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/ID.png"), "ID" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/IE.png"), "IE" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/IL.png"), "IL" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/IN.png"), "IN" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/IQ.png"), "IQ" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/IS.png"), "IS" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/IT.png"), "IT" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/JO.png"), "JO" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/JP.png"), "JP" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/KR.png"), "KR" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/LB.png"), "LB" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/LK.png"), "LK" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/LU.png"), "LU" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/MX.png"), "MX" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/MY.png"), "MY" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/NL.png"), "NL" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/NO.png"), "NO" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/NP.png"), "NP" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/NZ.png"), "NZ" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/OM.png"), "OM" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/PE.png"), "PE" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/PH.png"), "PH" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/PK.png"), "PK" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/PL.png"), "PL" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/PT.png"), "PT" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/QA.png"), "QA" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/SA.png"), "SA" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/SE.png"), "SE" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/SG.png"), "SG" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/SI.png"), "SK" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/TH.png"), "TH" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/TR.png"), "TR" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/TW.png"), "TW" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/US.png"), "US" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/UY.png"), "UY" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/VE.png"), "VE" );
    currencybox->insertItem ( QPixmap ( "/opt/QtPalmtop/pics/flags/ZA.png"), "ZA" );

    layout = new QVBoxLayout ( this );
    layout->addWidget ( currencylabel );
    layout->addWidget ( currencybox );
  }





