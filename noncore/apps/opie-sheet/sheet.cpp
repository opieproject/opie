/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*
 * Opie Sheet (formerly Sheet/Qt)
 * by Serdar Ozler <sozler@sitebest.com>
 */

#include "sheet.h"

#include <qmessagebox.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define DEFAULT_COL_WIDTH 50

Sheet::Sheet(int numRows, int numCols, QWidget *parent)
      :QTable(numRows, numCols, parent)
{
  defaultBorders.right=defaultBorders.bottom=QPen(Qt::gray, 1, Qt::SolidLine);
  defaultCellData.data="";
  defaultCellData.background=QBrush(Qt::white, Qt::SolidPattern);
  defaultCellData.alignment=(Qt::AlignmentFlags)(Qt::AlignLeft | Qt::AlignTop);
  defaultCellData.fontColor=Qt::black;
  defaultCellData.font=font();
  defaultCellData.borders=defaultBorders;

  clicksLocked=FALSE;
  selectionNo=-1;
  setSelectionMode(QTable::Single);

  sheetData.setAutoDelete(TRUE);
  clipboardData.setAutoDelete(TRUE);
  for (int i=0; i<numCols; ++i)
    horizontalHeader()->setLabel(i, getHeaderString(i+1), DEFAULT_COL_WIDTH);


  connect(this, SIGNAL(currentChanged(int,int)), this, SLOT(slotCellSelected(int,int)));
  connect(this, SIGNAL(valueChanged(int,int)), this, SLOT(slotCellChanged(int,int)));
}

Sheet::~Sheet()
{
}

typeCellData *Sheet::findCellData(int row, int col)
{
  typeCellData *tempCellData;
  for (tempCellData=sheetData.first(); tempCellData; tempCellData=sheetData.next())
    {
	if (tempCellData->row==row && tempCellData->col==col) return tempCellData;
    }
  return NULL;
}

void Sheet::slotCellSelected(int row, int col)
{
  typeCellData *cellData=findCellData(row, col);
  if (cellData)
  {
    emit currentDataChanged(cellData->data);
  }else
    emit currentDataChanged("");
}

typeCellData *Sheet::createCellData(int row, int col)
{
  if (row<0 || col<0) return NULL;
  typeCellData *cellData=new typeCellData;
  cellData->row=row;
  cellData->col=col;
  cellData->data=defaultCellData.data;
  cellData->borders=defaultCellData.borders;
  cellData->alignment=defaultCellData.alignment;
  cellData->font=defaultCellData.font;
  cellData->fontColor=defaultCellData.fontColor;
  cellData->background=defaultCellData.background;
  sheetData.append(cellData);
  return cellData;
}

void Sheet::slotCellChanged(int row, int col)
{
  typeCellData *cellData=findCellData(row, col);
  if (!cellData) cellData=createCellData(row, col);
  if (cellData) cellData->data=text(row, col);
  for (cellData=sheetData.first(); cellData; cellData=sheetData.next())
  {
	// modified by Toussis Manolis koppermind@panafonet.gr
	// the parser was crashing if there were no closed parenthesis.
	int w1,ii=0;
	for(w1=0;w1<=(int)text(row, col).length();w1++)
	{
		if(text(row,col)[w1]=='(') ii++;
		if(text(row,col)[w1]==')') ii--;
	};
	if(ii==0) setText(cellData->row, cellData->col, dataParser(findCellName(cellData->row, cellData->col), cellData->data));
	//end of modification
	// old was plain:
	//setText(cellData->row, cellData->col, dataParser(findCellName(cellData->row, cellData->col), cellData->data));
  };
  emit sheetModified();
}


void Sheet::ReCalc(void)
{
	typeCellData* cellData;
	for (cellData=sheetData.first(); cellData; cellData=sheetData.next())
    	{
		//printf("cellchanged:%d, %d\r\n",cellData->row,cellData->col);

		slotCellChanged(cellData->row,cellData->col);
	};
};




void Sheet::swapCells(int row1, int col1, int row2, int col2)
{
  typeCellData *cellData1=findCellData(row1, col1), *cellData2=findCellData(row2, col2);
  if (!cellData1) cellData1=createCellData(row1, col1);
  if (!cellData2) cellData2=createCellData(row2, col2);
  if (cellData1 && cellData2)
  {
    QString tempData(cellData1->data);
    cellData1->data=cellData2->data;
    cellData2->data=tempData;
    setText(cellData1->row, cellData1->col, dataParser(findCellName(cellData1->row, cellData1->col), cellData1->data));
    setText(cellData2->row, cellData2->col, dataParser(findCellName(cellData2->row, cellData2->col), cellData2->data));
    emit sheetModified();
  }
}

QString Sheet::getParameter(const QString &parameters, int paramNo, bool giveError, const QString funcName)
{
  QString params(parameters);
  int position;
  for (int i=0; i<paramNo; ++i)
  {
    position=params.find(',');
    if (position<0)
    {
      if (giveError) QMessageBox::critical(this, tr("Error"), tr("Too few arguments to function '"+funcName+'\''));
      //printf("params:%s\r\n",parameters.ascii());
      return QString(NULL);
    }
    params=params.mid(position+1);
  }
  position=params.find(',');
  if (position<0) return params;
  return params.left(position);
}

bool Sheet::findRange(const QString &variable1, const QString &variable2, int *row1, int *col1, int *row2, int *col2)
{
  int row, col;
  if (!findRowColumn(variable1, row1, col1, FALSE) || !findRowColumn(variable2, row2, col2, FALSE)) return FALSE;
  if (*row1>*row2)
  {
    row=*row1;
    *row1=*row2;
    *row2=row;
  }
  if (*col1>*col2)
  {
    col=*col1;
    *col1=*col2;
    *col2=col;
  }
  return TRUE;
}

bool Sheet::findRowColumn(const QString &variable, int *row, int *col, bool giveError)
{
  int position=variable.find(QRegExp("\\d"));
  if (position<1)
  {
    if (giveError) QMessageBox::critical(this, tr("Error"), tr("Invalid variable: '"+variable+'\''));
    return FALSE;
  }
  *row=variable.mid(position).toInt()-1;
  *col=getHeaderColumn(variable.left(position))-1;
  return TRUE;
}

QString Sheet::calculateVariable(const QString &variable)
{
  bool ok;
  printf("calculateVariable=%s,len=%d\r\n",variable.ascii(),variable.length());
  if(variable.left(1)=="\"") return QString(variable.mid(1,variable.length()-2));
  double tempResult=variable.toDouble(&ok);
  if (ok)
  {
	if(tempResult!=0.0)
	{
		return QString::number(tempResult);
	}
	else
	{
		if(variable!="0" || variable!="0.0") return QString(variable); // hereis a string variable
		return QString::number(tempResult);
	};
  };

  int row, col;
  if(findRowColumn(variable, &row, &col, FALSE)) return dataParser(variable, text(row,col));
  //return (findRowColumn(variable, &row, &col, TRUE) ? dataParser(variable, text(row, col)) : 0);
  return QString(variable);
}

double Sheet::BesselI0(double x)
{
	//Returns the modi ed Bessel function I0(x) for any real x.
	double ax,ans;
	double y;
	if ((ax=fabs(x)) < 3.75)
	{
		y=x/3.75;
		y*=y;
		ans=1.0+y*(3.5156229+y*(3.0899424+y*(1.2067492 +y*(0.2659732+y*(0.360768e-1+y*0.45813e-2)))));
	}else
	{
		y=3.75/ax;
		ans=(exp(ax)/sqrt(ax))*(0.39894228+y*(0.1328592e-1 +y*(0.225319e-2+y*(-0.157565e-2+y*(0.916281e-2 +y*(-0.2057706e-1+y*(0.2635537e-1+y*(-0.1647633e-1 +y*0.392377e-2))))))));
	}
	return ans;
};

double Sheet::BesselI1(double x)
{
	double ax,ans;
	double y;
	if ((ax=fabs(x)) < 3.75)
	{
		y=x/3.75;
		y*=y;
		ans=ax*(0.5+y*(0.87890594+y*(0.51498869+y*(0.15084934 +y*(0.2658733e-1+y*(0.301532e-2+y*0.32411e-3))))));
	} else
	{
		y=3.75/ax;
		ans=0.2282967e-1+y*(-0.2895312e-1+y*(0.1787654e-1 -y*0.420059e-2)); ans=0.39894228+y*(-0.3988024e-1+y*(-0.362018e-2 +y*(0.163801e-2+y*(-0.1031555e-1+y*ans))));
		ans *= (exp(ax)/sqrt(ax));
	}
	return x < 0.0 ? -ans : ans;
};

double Sheet::BesselI(int n, double x)
{
	double ACC=40.0;
	double BIGNO=1.0e10;
	double BIGNI=1.0e-10;
	int j;
	double bi,bim,bip,tox,ans;
	if (n < 2) return 0.0;
	if (x == 0.0) return 0.0; else
		{
			tox=2.0/fabs(x);
			bip=ans=0.0;
			bi=1.0;
			for (j=2*(n+(int) sqrt(ACC*n));j>0;j--)
			{
				bim=bip+j*tox*bi;
				bip=bi;
				bi=bim;
				if (fabs(bi) > BIGNO)
					{
						ans *= BIGNI;
						bi *= BIGNI;
						bip *= BIGNI;
					}
				if (j == n) ans=bip;
			}
		ans *= BesselI0(x)/bi;
		return x < 0.0 && (n & 1) ? -ans : ans;
		}
};

double Sheet::BesselK0(double x)
{
	double y,ans;
	if (x <= 2.0)
	{
		y=x*x/4.0;
		ans=(-log(x/2.0)*BesselI0(x))+(-0.57721566+y*(0.42278420 +y*(0.23069756+y*(0.3488590e-1+y*(0.262698e-2 +y*(0.10750e-3+y*0.74e-5))))));
	} else
	{
		y=2.0/x;
		ans=(exp(-x)/sqrt(x))*(1.25331414+y*(-0.7832358e-1 +y*(0.2189568e-1+y*(-0.1062446e-1+y*(0.587872e-2 +y*(-0.251540e-2+y*0.53208e-3))))));
	}
return ans;
};

double Sheet::BesselK1(double x)
{
	double y,ans;
	if (x <= 2.0)
	{
		y=x*x/4.0;
		ans=(log(x/2.0)*BesselI1(x))+(1.0/x)*(1.0+y*(0.15443144 +y*(-0.67278579+y*(-0.18156897+y*(-0.1919402e-1 +y*(-0.110404e-2+y*(-0.4686e-4)))))));
	} else
	{
		y=2.0/x;
		ans=(exp(-x)/sqrt(x))*(1.25331414+y*(0.23498619 +y*(-0.3655620e-1+y*(0.1504268e-1+y*(-0.780353e-2 +y*(0.325614e-2+y*(-0.68245e-3)))))));
	}
	return ans;
};

double Sheet::BesselK(int n, double x)
{
	int j;
	double bk,bkm,bkp,tox;
	if (n < 2) return 0.0;
	tox=2.0/x;
	bkm=BesselK0(x);
	bk=BesselK1(x);
	for (j=1;j<n;j++)
	{
		bkp=bkm+j*tox*bk;
		bkm=bk;
		bk=bkp;
	}
	return bk;
};

double Sheet::BesselJ0(double x)
{
	double ax,z;
	double xx,y,ans,ans1,ans2;
	if ((ax=fabs(x)) < 8.0)
	{
		y=x*x;
		ans1=57568490574.0+y*(-13362590354.0+y*(651619640.7 +y*(-11214424.18+y*(77392.33017+y*(-184.9052456)))));
		ans2=57568490411.0+y*(1029532985.0+y*(9494680.718 +y*(59272.64853+y*(267.8532712+y*1.0))));
		ans=ans1/ans2;
	} else
	{
		z=8.0/ax;
		y=z*z;
		xx=ax-0.785398164;
		ans1=1.0+y*(-0.1098628627e-2+y*(0.2734510407e-4 +y*(-0.2073370639e-5+y*0.2093887211e-6)));
		ans2 = -0.1562499995e-1+y*(0.1430488765e-3 +y*(-0.6911147651e-5+y*(0.7621095161e-6 -y*0.934935152e-7)));
		ans=sqrt(0.636619772/ax)*(cos(xx)*ans1-z*sin(xx)*ans2);
	}
	return ans;
};

double Sheet::BesselY0(double x)
{
	double z;
	double xx,y,ans,ans1,ans2;
	if (x < 8.0)
	{
		y=x*x;
		ans1 = -2957821389.0+y*(7062834065.0+y*(-512359803.6 +y*(10879881.29+y*(-86327.92757+y*228.4622733))));
		ans2=40076544269.0+y*(745249964.8+y*(7189466.438 +y*(47447.26470+y*(226.1030244+y*1.0))));
		ans=(ans1/ans2)+0.636619772*BesselJ0(x)*log(x);
	} else
	{
		z=8.0/x;
		y=z*z;
		xx=x-0.785398164;
		ans1=1.0+y*(-0.1098628627e-2+y*(0.2734510407e-4 +y*(-0.2073370639e-5+y*0.2093887211e-6)));
		ans2 = -0.1562499995e-1+y*(0.1430488765e-3 +y*(-0.6911147651e-5+y*(0.7621095161e-6 +y*(-0.934945152e-7))));
		ans=sqrt(0.636619772/x)*(sin(xx)*ans1+z*cos(xx)*ans2);
	}
	return ans;
};

double Sheet::BesselJ1(double x)
{
	double ax,z;
	double xx,y,ans,ans1,ans2;
	if ((ax=fabs(x)) < 8.0)
	{
		y=x*x;
		ans1=x*(72362614232.0+y*(-7895059235.0+y*(242396853.1 +y*(-2972611.439+y*(15704.48260+y*(-30.16036606))))));
		ans2=144725228442.0+y*(2300535178.0+y*(18583304.74 +y*(99447.43394+y*(376.9991397+y*1.0))));
		ans=ans1/ans2;
	} else
	{
		z=8.0/ax; y=z*z; xx=ax-2.356194491;
		ans1=1.0+y*(0.183105e-2+y*(-0.3516396496e-4 +y*(0.2457520174e-5+y*(-0.240337019e-6))));
		ans2=0.04687499995+y*(-0.2002690873e-3 +y*(0.8449199096e-5+y*(-0.88228987e-6 +y*0.105787412e-6)));
		ans=sqrt(0.636619772/ax)*(cos(xx)*ans1-z*sin(xx)*ans2);
		if (x < 0.0) ans = -ans;
	}
	return ans;
};

double Sheet::BesselY1(double x)
{
	double z;
	double xx,y,ans,ans1,ans2;
	if (x < 8.0)
	{
		y=x*x;
		ans1=x*(-0.4900604943e13+y*(0.1275274390e13 +y*(-0.5153438139e11+y*(0.7349264551e9 +y*(-0.4237922726e7+y*0.8511937935e4)))));
		ans2=0.2499580570e14+y*(0.4244419664e12 +y*(0.3733650367e10+y*(0.2245904002e8 +y*(0.1020426050e6+y*(0.3549632885e3+y)))));
		ans=(ans1/ans2)+0.636619772*(BesselJ1(x)*log(x)-1.0/x);
	} else
	{
		z=8.0/x;
		y=z*z;
		xx=x-2.356194491;
		ans1=1.0+y*(0.183105e-2+y*(-0.3516396496e-4 +y*(0.2457520174e-5+y*(-0.240337019e-6))));
		ans2=0.04687499995+y*(-0.2002690873e-3 +y*(0.8449199096e-5+y*(-0.88228987e-6 +y*0.105787412e-6)));
		ans=sqrt(0.636619772/x)*(sin(xx)*ans1+z*cos(xx)*ans2);
	}
	return ans;
};

double Sheet::BesselY(int n, double x)
{
	int j;
	double by,bym,byp,tox;
	if (n < 2) return 0.0;
	tox=2.0/x;
	by=BesselY1(x);
	bym=BesselY0(x);
	for (j=1;j<n;j++)
	{
		byp=j*tox*by-bym;
		bym=by;
		by=byp;
	}
	return by;
};

double Sheet::BesselJ(int n, double x)
{
	double ACC=40.0;
	double BIGNO=1.0e10;
	double BIGNI=1.0e-10;
 	int j,jsum,m;
	double ax,bj,bjm,bjp,sum,tox,ans;
	if (n < 2) return 0.0;
	ax=fabs(x);
	if (ax == 0.0) return 0.0;
		else if (ax > (double) n)
		{
			tox=2.0/ax;
			bjm=BesselJ0(ax);
			bj=BesselJ1(ax);
			for (j=1;j<n;j++)
			{
				bjp=j*tox*bj-bjm;
				bjm=bj;
				bj=bjp;
			}
			ans=bj;
		} else
		{
			tox=2.0/ax;
			m=2*((n+(int) sqrt(ACC*n))/2);
			jsum=0;
			bjp=ans=sum=0.0;
			bj=1.0;
			for (j=m;j>0;j--)
			{
				bjm=j*tox*bj-bjp;
				bjp=bj;
				bj=bjm;
				if (fabs(bj) > BIGNO)
				{
					bj *= BIGNI;
					bjp *= BIGNI;
					ans *= BIGNI;
					sum *= BIGNI;
				}
				if (jsum) sum += bj;
				jsum=!jsum;
				if (j == n) ans=bjp;
			}
			sum=2.0*sum-bj;
			ans /= sum;
		}
	return x < 0.0 && (n & 1) ? -ans : ans;
};

double Sheet::GammaLn(double xx)
{
	double x,y,tmp,ser;
	static double cof[6]={76.18009172947146,-86.50532032941677, 24.01409824083091,-1.231739572450155, 0.1208650973866179e-2,-0.5395239384953e-5};
	int j;
	y=x=xx;
	tmp=x+5.5;
	tmp -= (x+0.5)*log(tmp);
	ser=1.000000000190015;
	for (j=0;j<=5;j++) ser += cof[j]/++y;
	return -tmp+log(2.5066282746310005*ser/x);
};

double Sheet::Factorial(double n)
{
	if (n < 0) return 0.0;
	if (n > 100) return 0.0;
	return exp(GammaLn(n+1.0));
};

double Sheet::GammaP(double a, double x)
{
// returns GammaP(a,x)
//void gcf(float *gammcf, float a, float x, float *gln);
//void gser(float *gamser, float a, float x, float *gln);
	double gamser,gammcf,gln;
	if (x < 0.0 || a <= 0.0) return 0.0;//error
	if (x < (a+1.0))
	{
		GammaSeries(&gamser,a,x,&gln);
		return gamser;
	}else
	{
		GammaContinuedFraction(&gammcf,a,x,&gln);
		return 1.0-gammcf;
	}
};

double Sheet::GammaQ(double a,double x)
{
	//returns GammaQ(a,x)=1.0 - GammaP(a,x);
	return (1.0-GammaP(a,x));
};


void Sheet::GammaSeries(double *gamser, double a, double x, double *gln)
{
	double EPS=3.0e-7;
	int ITMAX=100;
	int n;
	double sum,del,ap;
	*gln=GammaLn(a);
	if (x <= 0.0)
	{
		if (x < 0.0) return;//error
		*gamser=0.0;
		return;
	} else
	{
		ap=a;
		del=sum=1.0/a;
		for (n=1;n<=ITMAX;n++)
		{
			++ap;
			del *= x/ap;
			sum += del;
			if (fabs(del) < fabs(sum)*EPS)
			{
				*gamser=sum*exp(-x+a*log(x)-(*gln));
				return;
			}
		} return;
		return;
	}
};


void Sheet::GammaContinuedFraction(double *gammcf, double a, double x, double *gln)
{
	double EPS=3.0e-7;
	double FPMIN=1.0e-30;
	int ITMAX=100;
	int i;
	double an,b,c,d,del,h;
	*gln=GammaLn(a);
	b=x+1.0-a;
	c=1.0/FPMIN;
	d=1.0/b; h=d;
	for (i=1;i<=ITMAX;i++)
	{
		an = -i*(i-a);
		b += 2.0; d=an*d+b;
		if (fabs(d) < FPMIN) d=FPMIN;
		c=b+an/c;
		if (fabs(c) < FPMIN) c=FPMIN;
		d=1.0/d; del=d*c; h *= del;
		if (fabs(del-1.0) < EPS) break;
	}
	if (i > ITMAX) return;
	*gammcf=exp(-x+a*log(x)-(*gln))*h;
};

double Sheet::ErrorFunction(double x)
{
	return x < 0.0 ? -GammaP(0.5,x*x) : GammaP(0.5,x*x);
};

double Sheet::ErrorFunctionComplementary(double x)
{
	return x < 0.0 ? 1.0+GammaP(0.5,x*x) : GammaQ(0.5,x*x);
};

double Sheet::Beta(double z, double w)
{
	return exp(GammaLn(z)+GammaLn(w)-GammaLn(z+w));
};


double Sheet::BetaContinuedFraction(double a, double b, double x)
{
	int MAXIT=100;
	double EPS=3.0e-7;
	double FPMIN=1.0e-30;
	int m,m2;
	double aa,c,d,del,h,qab,qam,qap;
	qab=a+b;
	qap=a+1.0; qam=a-1.0; c=1.0;
	d=1.0-qab*x/qap;
	if (fabs(d) < FPMIN) d=FPMIN;
	d=1.0/d; h=d;
	for (m=1;m<=MAXIT;m++)
	{
		m2=2*m; aa=m*(b-m)*x/((qam+m2)*(a+m2));
		d=1.0+aa*d;
		if (fabs(d) < FPMIN) d=FPMIN;
		c=1.0+aa/c;
		if (fabs(c) < FPMIN) c=FPMIN;
		d=1.0/d; h *= d*c;
		aa = -(a+m)*(qab+m)*x/((a+m2)*(qap+m2)); d=1.0+aa*d;
		if (fabs(d) < FPMIN) d=FPMIN;
		c=1.0+aa/c;
		if (fabs(c) < FPMIN) c=FPMIN; d=1.0/d;
		del=d*c; h *= del;
		if (fabs(del-1.0) < EPS) break;
	}
	if (m > MAXIT) return 0.0;
	return h;
};

double Sheet::BetaIncomplete(double a, double b, double x)
{
	double bt;
	if (x < 0.0 || x > 1.0) return 0.0;
	if (x == 0.0 || x == 1.0) bt=0.0; else
		bt=exp(GammaLn(a+b)-GammaLn(a)-GammaLn(b)+a*log(x)+b*log(1.0-x));
	if (x < (a+1.0)/(a+b+2.0)) return bt*BetaContinuedFraction(a,b,x)/a; else
		return 1.0-bt*BetaContinuedFraction(b,a,1.0-x)/b;
};



double Sheet::functionSum(const QString &param1, const QString &param2)
{
  int row1, col1, row2, col2, row, col;
  double result=0, tempResult;
  bool ok;
  if (findRange(param1, param2, &row1, &col1, &row2, &col2))
  {
	for (row=row1; row<=row2; ++row)
    	for (col=col1; col<=col2; ++col)
    	{
      		tempResult=text(row, col).toDouble(&ok);
      		if (ok) result+=tempResult;
	}
  return result;
  }else
  {
	double d1=0,d2=0;
	d1=calculateVariable(param1).toDouble(&ok);
	d2=calculateVariable(param2).toDouble(&ok);
	return(d1+d2);
  };
  return 0;
}

QString Sheet::functionIndex(const QString &param1, const QString &param2, int indx)
{
  int row1, col1, row2, col2, row, col;
  if (!findRange(param1, param2, &row1, &col1, &row2, &col2)) return 0;
  int ii=1;
  for (col=col1; col<=col2; ++col)
    for (row=row1; row<=row2; ++row)
    {
      if(ii==indx) return text(row,col);
      ii++;
    }
  return QString("");
}



double Sheet::functionVariancePopulation(const QString &param1, const QString &param2)
{
  int row1, col1, row2, col2, row, col;
  if (!findRange(param1, param2, &row1, &col1, &row2, &col2)) return 0;
  double avg1=functionAvg(param1,param2);
  double result=0, tempResult;
  int count1=0;
  bool ok;
  for (row=row1; row<=row2; ++row)
    for (col=col1; col<=col2; ++col)
    {
      tempResult=text(row, col).toDouble(&ok);
      if (ok) { result=result + (tempResult - avg1)*(tempResult - avg1); count1++;};
    }
  if(count1>0) result=result/double(count1); else result=0.0;
  return result;
};

double Sheet::functionVariance(const QString &param1, const QString &param2)
{
  int row1, col1, row2, col2, row, col;
  if (!findRange(param1, param2, &row1, &col1, &row2, &col2)) return 0;
  double avg1=functionAvg(param1,param2);
  double result=0, tempResult;
  int count1=0;
  bool ok;
  for (row=row1; row<=row2; ++row)
    for (col=col1; col<=col2; ++col)
    {
      tempResult=text(row, col).toDouble(&ok);
      if (ok) { result=result + (tempResult - avg1)*(tempResult - avg1); count1++;};
    }
  if(count1>1) result=result/double(count1-1); else result=0.0;
  return result;
};

double Sheet::functionSkew(const QString &param1, const QString &param2)
{
  int row1, col1, row2, col2, row, col;
  if (!findRange(param1, param2, &row1, &col1, &row2, &col2)) return 0;
  double avg1=functionAvg(param1,param2);
  double var1=sqrt(functionVariancePopulation(param1,param2));
  if(var1==0.0) return 0.0;
  double result=0, tempResult;
  int count1=0;
  bool ok;
  for (row=row1; row<=row2; ++row)
    for (col=col1; col<=col2; ++col)
    {
      tempResult=text(row, col).toDouble(&ok);
      if (ok)
      {
      result=result + (tempResult - avg1)*(tempResult - avg1)*(tempResult - avg1)/(var1*var1*var1);
      count1++;
      };
    }
  if(count1>0) result=result/double(count1); else result=0.0;
  return result;
};

double Sheet::functionKurt(const QString &param1, const QString &param2)
{
  int row1, col1, row2, col2, row, col;
  if (!findRange(param1, param2, &row1, &col1, &row2, &col2)) return 0;
  double avg1=functionAvg(param1,param2);
  double var1=sqrt(functionVariancePopulation(param1,param2));
  if(var1==0.0) return 0.0;
  double result=0, tempResult;
  int count1=0;
  bool ok;
  for (row=row1; row<=row2; ++row)
    for (col=col1; col<=col2; ++col)
    {
      tempResult=text(row, col).toDouble(&ok);
      if (ok)
      {
      result=result + (tempResult - avg1)*(tempResult - avg1)*
      				(tempResult - avg1)*(tempResult - avg1)/(var1*var1*var1*var1);
      count1++;
      };
    }
  if(count1>0) result=result/double(count1)-3.0; else result=0.0;
  return result;
};



double Sheet::functionSumSQ(const QString &param1, const QString &param2)
{
  int row1, col1, row2, col2, row, col;
  double result=0, tempResult;
  bool ok;
  if (findRange(param1, param2, &row1, &col1, &row2, &col2))
  {
	for (row=row1; row<=row2; ++row)
    	for (col=col1; col<=col2; ++col)
    	{
      		tempResult=text(row, col).toDouble(&ok);
      		if (ok) result+=tempResult*tempResult;
	}
  	return result;
  }else
  {
	double d1=0,d2=0;
	d1=calculateVariable(param1).toDouble(&ok);
	d2=calculateVariable(param2).toDouble(&ok);
	return(d1*d1+d2*d2);
  };
  return 0;
}



double Sheet::functionMin(const QString &param1, const QString &param2)
{
  int row1, col1, row2, col2, row, col;
  double min=0, tempMin;
  bool ok, init=FALSE;
  if (findRange(param1, param2, &row1, &col1, &row2, &col2))
  {
	for (row=row1; row<=row2; ++row)
	for (col=col1; col<=col2; ++col)
	{
		tempMin=text(row, col).toDouble(&ok);
		if (ok && (!init || tempMin<min))
		{
			min=tempMin;
			init=TRUE;
		}
		}
	return min;
  }else
  {
	double d1=0,d2=0;
	d1=calculateVariable(param1).toDouble(&ok);
	d2=calculateVariable(param2).toDouble(&ok);
	if(d1<d2) return(d1); else return(d2);
  };
  return 0;
}

double Sheet::functionMax(const QString &param1, const QString &param2)
{
  int row1, col1, row2, col2, row, col;
  double max=0, tempMax;
  bool ok, init=FALSE;
  if (findRange(param1, param2, &row1, &col1, &row2, &col2))
  {
	for (row=row1; row<=row2; ++row)
	for (col=col1; col<=col2; ++col)
	{
		tempMax=text(row, col).toDouble(&ok);
		if (ok && (!init || tempMax>max))
		{
			max=tempMax;
			init=TRUE;
		}
	};
	return max;
  }else
  {
	double d1=0,d2=0;
	d1=calculateVariable(param1).toDouble(&ok);
	d2=calculateVariable(param2).toDouble(&ok);
	if(d1>d2) return(d1); else return(d2);
  };
  return 0;
}

double Sheet::functionAvg(const QString &param1, const QString &param2)
{
  double resultSum=functionSum(param1, param2), resultCount=functionCount(param1, param2);
  return (resultCount>0 ? resultSum/resultCount : 0);
}

double Sheet::functionCount(const QString &param1, const QString &param2)
{
  int row1, col1, row2, col2, row, col;
  int divider=0;
  bool ok;
  if (findRange(param1, param2, &row1, &col1, &row2, &col2))
  {
	for (row=row1; row<=row2; ++row)
	for (col=col1; col<=col2; ++col)
	{
		text(row, col).toDouble(&ok);
		if (ok) ++divider;
	};
	return divider;
  }else
  {
	double d1=0,d2=0;int ii=0;
	d1=calculateVariable(param1).toDouble(&ok);
	if (ok) ii++;
	d2=calculateVariable(param2).toDouble(&ok);
	if (ok) ii++;
	return(ii);
  };
  return 0;
}

double Sheet::functionCountIf(const QString &param1, const QString &param2, const QString &param3)
{
  int row1, col1, row2, col2, row, col;
  if (!findRange(param1, param2, &row1, &col1, &row2, &col2)) return 0;
  //same as count except check if each field is equal to param3
  int divider=0;
  QString s2;
  bool ok;
  s2=calculateVariable(param3);
  for (row=row1; row<=row2; ++row)
    for (col=col1; col<=col2; ++col)
    {
      text(row, col).toDouble(&ok);
      if (ok && (s2==text(row,col)) ) ++divider;
    }
  return divider;
}


QString Sheet::calculateFunction(const QString &func, const QString &parameters, int NumOfParams)
{
  bool ok;
  double val1=0.0,val2=0.0,val3=0.0;
  long int vali=0;
  int w1,w2;
  int row,col;
  QString s1,s2;
//basic functions
  QString function;
  function=func.upper();
  if (function=="+")
  {
    s1=calculateVariable(getParameter(parameters, 0));
    s2=calculateVariable(getParameter(parameters, 1));
    val1=s1.toDouble(&ok)+s2.toDouble(&ok);
    return QString::number(val1);

   };
  if (function=="-")
  {
    s1=calculateVariable(getParameter(parameters, 0));
    s2=calculateVariable(getParameter(parameters, 1));
    val1=s1.toDouble(&ok)-s2.toDouble(&ok);
    return QString::number(val1);
   };
  if (function=="*")
  {
    val1=calculateVariable(
    	getParameter(parameters, 0)).toDouble(&ok)
		*calculateVariable(getParameter(parameters, 1)).toDouble(&ok);
    return QString::number(val1);
   };
  if (function=="/")
  {
    val1=calculateVariable(getParameter(parameters, 0)).toDouble(&ok);
    val2=calculateVariable(getParameter(parameters, 1)).toDouble(&ok);
    if(val2==0.0) return QString("Err101");
    val1=val1/val2;
    return QString::number(val1);
    };
  if (function==">")
  {
    val1=calculateVariable(getParameter(parameters, 0)).toDouble(&ok);
    val2=calculateVariable(getParameter(parameters, 1)).toDouble(&ok);
    if(val1>val2) return QString::number(1); else return QString::number(0);
    };
  if (function=="<")
  {
    val1=calculateVariable(getParameter(parameters, 0)).toDouble(&ok);
    val2=calculateVariable(getParameter(parameters, 1)).toDouble(&ok);
    if(val1<val2) return QString::number(1); else return QString::number(0);
    };
  if (function==">=")
  {
    val1=calculateVariable(getParameter(parameters, 0)).toDouble(&ok);
    val2=calculateVariable(getParameter(parameters, 1)).toDouble(&ok);
    if(val1>=val2) return QString::number(1); else return QString::number(0);
    };
  if (function=="<=")
  {
    val1=calculateVariable(getParameter(parameters, 0)).toDouble(&ok);
    val2=calculateVariable(getParameter(parameters, 1)).toDouble(&ok);
    if(val1<=val2) return QString::number(1); else return QString::number(0);
    };
  if (function=="!=")
  {
    val1=calculateVariable(getParameter(parameters, 0)).toDouble(&ok);
    val2=calculateVariable(getParameter(parameters, 1)).toDouble(&ok);
    if(val1!=val2) return QString::number(1); else return QString::number(0);
    };
  if (function=="=="||function=="=")
  {
    val1=calculateVariable(getParameter(parameters, 0)).toDouble(&ok);
    val2=calculateVariable(getParameter(parameters, 1)).toDouble(&ok);
    if(val1==val2) return QString::number(1); else return QString::number(0);
    };

    //LOGICAL / INFO
   if (function=="ISBLANK")
   {
	if(findRowColumn(getParameter(parameters, 0), &row, &col, FALSE))
	{
		if(text(row,col).length()==0) val1=1; else val1=0;
	}else
	{
		if(findRowColumn(calculateVariable(getParameter(parameters, 0)), &row,&col, FALSE))
		{
			if(text(row,col).length()==0) val1=1; else val1=0;
		}else
		{
			val1=0;
		};
	};
	return QString::number(val1);
   };


   if (function=="ISNUMBER")
   {
	if(findRowColumn(getParameter(parameters, 0, TRUE, function), &row, &col, FALSE))
	{
		val1=text(row,col).toDouble(&ok);
		if(ok) val1=1; else val1=0;
	}else
	{
		if(findRowColumn(calculateVariable(getParameter(parameters, 0, TRUE, function)), &row,&col, FALSE))
		{
			val1=text(row,col).toDouble(&ok);
			if(ok) val1=1; else val1=0;
		}else
		{
			val1=0;
		};
	};
	return QString::number(val1);
   };
   if (function=="AND")
   {
	vali=calculateVariable(getParameter(parameters, 0, TRUE, function)).toInt(&ok)
		& calculateVariable(getParameter(parameters, 1, TRUE, function)).toInt(&ok);
	return QString::number(vali);
   };
   if (function=="OR")
   {
	vali=calculateVariable(getParameter(parameters, 0, TRUE, function)).toInt(&ok)
		| calculateVariable(getParameter(parameters, 1, TRUE, function)).toInt(&ok);
	return QString::number(vali);
   };
   if (function=="NOT")
   {
	vali=!calculateVariable(getParameter(parameters, 0, TRUE, function)).toInt(&ok);
	return QString::number(vali);
   };

   // MATHEMATICAL FUNCTIONS
   if (function=="ABS")
   {
	val1=fabs(calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok));
	return QString::number(val1);
   };
   if (function=="ACOS")
   {
	val1=acos(calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok));
	return QString::number(val1);
   };
   if (function=="ACOSH")
   {
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	if(val1<1.0) return QString::number(0);
	val1=acosh(val1);
	return QString::number(val1);
   };
   if (function=="ASIN")
   {
	val1=asin(calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok));
	return QString::number(val1);
   };
   if (function=="ASINH")
   {
	val1=asinh(calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok));
	return QString::number(val1);
   };
   if (function=="ATAN")
   {
	val1=atan(calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok));
	return QString::number(val1);
   };
   if (function=="ATAN2")
   {
	val1=atan2(calculateVariable(getParameter(parameters, 1, TRUE, function)).toDouble(&ok),
			calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok));
	return QString::number(val1);
   };
   if (function=="ATANH")
   {
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	if(val1<=-1.0 || val1>=1.0) return QString("Err101");
	val1=atanh(val1);
	return QString::number(val1);
   };
   if (function=="CEILING")
   {
	// rounds up param1 to specified accuracy param2
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	val2=calculateVariable(getParameter(parameters, 1, TRUE, function)).toDouble(&ok);
	if(val2==0.0) return QString::number(val1);
	val1=ceil(val1/val2)*val2;
	return QString::number(val1);
   };
   if (function=="COS")
   {
	val1=cos(calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok));
	return QString::number(val1);
   };
   if (function=="COSH")
   {
	val1=cosh(calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok));
	return QString::number(val1);
   };
   if (function=="DEGREES")
   {
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok)*180.0/M_PI;
	return QString::number(val1);
   };
   if (function=="EXP")
   {
	val1=exp(calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok));
	return QString::number(val1);
   };
   if (function=="FACT")
   {
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	val2=Factorial(val1);
	return QString::number(val2);
   };
   if (function=="FLOOR")
   {
	// rounds down param1 to specified accuracy param2
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	val2=calculateVariable(getParameter(parameters, 1, TRUE, function)).toDouble(&ok);
	if(val2==0.0) return QString::number(val1);
	val1=floor(val1/val2)*val2;
	return QString::number(val1);
   };
   if (function=="INT")
   {
	// rounds down param1
	val1=int(calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok));
	return QString::number(val1);
   };
   if (function=="EVEN")
   {
	//converts param1 to even
	vali=int(calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok));
	if(vali % 2 !=0) val1=vali+1; else val1=vali;
	return QString::number(val1);
   };
   if (function=="ODD")
   {
	//converts param1 to odd
	vali=int(calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok));
	if(vali % 2 !=0) val1=vali; else val1=vali+1;
	return QString::number(val1);
   };
   if (function=="ISEVEN")
   {
	//Is Even param1?
	vali=int(calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok));
	if(vali % 2 == 0) val1=1; else val1=0;
	return QString::number(val1);
   };
   if (function=="ISODD")
   {
	//Is odd param1?
	vali=int(calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok));
	if(vali % 2 == 0) val1=0; else val1=1;
	return QString::number(val1);
   };
   if (function=="LN")
   {
	// returns the natural logarithm of param1
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	if(val1<=0.0) return QString("Err101");
	val1=log(val1);
	return QString::number(val1);
   };
   if (function=="LOG10")
   {
	// returns the base-10 logarithm of param1
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	if(val1<=0.0) return QString("Err101");
	val1=log10(val1);
	return QString::number(val1);
   };
   if (function=="LOG")
   {
	// return the base-param2 logarithm of param1
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	val2=calculateVariable(getParameter(parameters, 1, TRUE, function)).toDouble(&ok);
	if(val1<=0.0 || val2<=0.0 ) return QString("Err101");
	val1=log(val1)/log(val2);
	return QString::number(val1);
   };
   if (function=="MOD")
   {
	// return the modulus of param1/param2
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	val2=calculateVariable(getParameter(parameters, 1, TRUE, function)).toDouble(&ok);
	if(val2==0.0) return QString("Err101");
	val1=(int(val1) % int(val2));
	return QString::number(val1);
   };
   if (function=="POWER")
   {
	// return the param1^param2
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	val2=calculateVariable(getParameter(parameters, 1, TRUE, function)).toDouble(&ok);
	if(val1<0.0 && (floor(val2)!=val2)) return QString("Err101");
	val1=pow(val1,val2);
	return QString::number(val1);
   };
   if (function=="PI")
   {
	return QString::number(M_PI);
   };
   if (function=="RADIANS")
   {
	// param1 deg->rad
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok)*M_PI/180.0;
	return QString::number(val1);
   };
   if (function=="RAND")
   {
	// retuns random number 0>x>1
	srand((unsigned int)time((time_t *)NULL));
	val1=double(rand())/double(RAND_MAX);
	return QString::number(val1);
   };
   if (function=="RANDBETWEEN")
   {
	// returns random number between param1>x>param2
	//TOFIX: this is not ok because I think results is always int related.
	srand((unsigned int)time((time_t *)NULL));
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	val2=calculateVariable(getParameter(parameters, 1, TRUE, function)).toDouble(&ok);
	val1=fmod(double(rand()),(val2-val1))+val1;
	return QString::number(val1);
   };
   if (function=="ROUND")
   {
	// rounds down param1 to specified digits param2 (positive decimal digits)
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	val2=calculateVariable(getParameter(parameters, 1, TRUE, function)).toDouble(&ok);
	val2=pow(10.0,-val2);
	val1=floor(val1/val2)*val2;
	return QString::number(val1);
   };
   if (function=="SIGN")
   {
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	if(val1>=0.0) return QString::number(1.0); else return QString::number(-1.0);
   };
   if (function=="CHGSGN")//changes sign (for unary operator)
   {
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	return QString::number((-1.0)*val1);
   };
   if (function=="SIN")
   {
	val1=sin(calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok));
	return QString::number(val1);
   };
   if (function=="SINH")
   {
	val1=sinh(calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok));
	return QString::number(val1);
   };
   if (function=="TAN")
   {
	val1=tan(calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok));
	return QString::number(val1);
   };
   if (function=="TANH")
   {
	val1=tanh(calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok));
	return QString::number(val1);
   };
   if (function=="SQRT")
   {
	val1=sqrt(calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok));
	return QString::number(val1);
   };


// STRING FUNCTIONS
   if (function=="CONCATENATE")
   {
	// concatenates strings together
	w1=0;
	s1="";
	while(getParameter(parameters, w1, FALSE, function)!="") //parse all params;
	{
		s1=s1+calculateVariable(getParameter(parameters, w1));
		w1++;
	};
	return QString(s1);
   };
   if (function=="EXACT")
   {
	// compare two string if they are exactly the same
	s1=calculateVariable(getParameter(parameters, 0, TRUE, function));
	s2=calculateVariable(getParameter(parameters, 1, TRUE, function));
	if(s1==s2) return QString::number(1); else return QString::number(0);
   };
   if (function=="FIND")
   {
	// finds param1 in param2 from pos param3 and after
	// returns -1 if not found
	s1=calculateVariable(getParameter(parameters, 0, TRUE, function));
	s2=calculateVariable(getParameter(parameters, 1, TRUE, function));
	vali=calculateVariable(getParameter(parameters, 2, TRUE, function)).toInt(&ok);
	val1=s2.find(s1,vali);
	return QString::number(val1);
   };
   if (function=="LEFT")
   {
	// returns the param2 left chars from param1 string
	s1=calculateVariable(getParameter(parameters, 0, TRUE, function));
	vali=calculateVariable(getParameter(parameters, 1, TRUE, function)).toInt(&ok);
	s2=s1.left(vali);
	return QString(s2);
   };
   if (function=="LEN")
   {
	// return the length of a string(param1)
	s1=calculateVariable(getParameter(parameters, 0, TRUE, function));
	return QString::number(s1.length());
   };
   if (function=="MID")
   {
	// returns the mid word of string param1 with start param2 and len param3
	s1=calculateVariable(getParameter(parameters, 0, TRUE, function));
	w1=calculateVariable(getParameter(parameters, 1, TRUE, function)).toInt(&ok);
	w2=calculateVariable(getParameter(parameters, 2, TRUE, function)).toInt(&ok);
	s2=s1.mid(w1,w2);
	return QString(s2);
   };
   if (function=="REPLACE")
   {
	//replace in param1 text in pos param2 and length param3 to newtext param4
	s1=calculateVariable(getParameter(parameters, 0, TRUE, function));
	w1=calculateVariable(getParameter(parameters, 1, TRUE, function)).toInt(&ok);
	w2=calculateVariable(getParameter(parameters, 2, TRUE, function)).toInt(&ok);
	s2=calculateVariable(getParameter(parameters, 3, TRUE, function));
	if(w1<0 || w2<0) return QString(s1);
	s1=s1.left(w2-1)+s2+s1.right(s1.length()-w1-w2);
	return QString(s1);
   };
   if (function=="REPT")
   {
	//repeats param1 string param2 times
	s1=calculateVariable(getParameter(parameters, 0, TRUE, function));
	w1=calculateVariable(getParameter(parameters, 1, TRUE, function)).toInt(&ok);
	for(w2=1;w2<=w1;w2++)
	{
		s2=s2.append(s1);
	};
	return QString(s2);
   };
   if (function=="RIGHT")
   {
	// returns the param2 right chars from param1 string
	s1=calculateVariable(getParameter(parameters, 0, TRUE, function));
	vali=calculateVariable(getParameter(parameters, 1, TRUE, function)).toInt(&ok);
	s2=s1.right(vali);
	return QString(s2);
   };
   if (function=="UPPER")
   {
	// returns the upper param1 string
	s1=calculateVariable(getParameter(parameters, 0, TRUE, function));
	s1=s1.upper();
	return QString(s1);
   };
   if (function=="LOWER")
   {
	// returns the lower param1 string
	s1=calculateVariable(getParameter(parameters, 0, TRUE, function));
	s1=s1.lower();
	return QString(s1);
   };
   if (function=="IF")
   {
	//usage: IF(param1,param2,param3)
	//returns param4 if true(param1)/   param5 if false(param1)
	val1=getParameter(parameters, 0, TRUE, function).toDouble(&ok);
	if(val1==1.0)
	{
		s1=calculateVariable(getParameter(parameters, 1, TRUE, function));
		return QString(s1);
	}else
	{
		s1=calculateVariable(getParameter(parameters, 2, TRUE, function));
		return QString(s1);
	};
   };
   if (function=="SUM")
   {
	//NumOfParams
	val2=0.0;
	for(w1=1;w1<=(NumOfParams/2);w1++)
	{
	val1=functionSum(getParameter(parameters, (w1-1)*2, FALSE, function), getParameter(parameters, (w1-1)*2+1, TRUE, function));
	val2=val2+val1;
	};
	if(NumOfParams%2==1)
	{
	val2=val2+calculateVariable(getParameter(parameters,NumOfParams-1,FALSE, function)).toDouble(&ok);
	};
	return QString::number(val2);
   };
   if (function=="INDEX")
   {
	s1=functionIndex(getParameter(parameters,0,TRUE,function), getParameter(parameters, 1, TRUE, function), getParameter(parameters,2,TRUE,function).toInt(&ok));
	return QString(s1);
   };
   if (function=="SUMSQ")
   {
	//NumOfParams
	val2=0.0;
	for(w1=1;w1<=(NumOfParams/2);w1++)
	{
	val1=functionSumSQ(getParameter(parameters, (w1-1)*2, FALSE, function), getParameter(parameters, (w1-1)*2+1, TRUE, function));
	val2=val2+val1;
	};
	if(NumOfParams%2==1)
	{
	val1=calculateVariable(getParameter(parameters,NumOfParams-1,FALSE, function)).toDouble(&ok);
	val2=val2+val1*val1;
	};
	return QString::number(val2);
   };
   if (function=="COUNT")
   {
	//NumOfParams
	val2=0.0;
	for(w1=1;w1<=(NumOfParams/2);w1++)
	{
	val1=functionCount(getParameter(parameters, (w1-1)*2, FALSE, function), getParameter(parameters, (w1-1)*2+1, TRUE, function));
	val2=val2+val1;
	};
	if(NumOfParams%2==1)
	{
	val1=calculateVariable(getParameter(parameters,NumOfParams-1,FALSE, function)).toDouble(&ok);
	if(ok) val2=val2+1;
	};
	return QString::number(val2);
   };
   if (function=="COUNTIF")
   {
	//NumOfParams
	val1=functionCountIf(getParameter(parameters, 0, TRUE, function), getParameter(parameters, 1, TRUE, function), getParameter(parameters, 2, TRUE, function));
	return QString::number(val1);
   };
   if (function=="MIN")
   {
	//NumOfParams
	val2=0.0;
	for(w1=1;w1<=(NumOfParams/2);w1++)
	{
	val1=functionMin(getParameter(parameters, (w1-1)*2, FALSE, function), getParameter(parameters, (w1-1)*2+1, TRUE, function));
	val2=val1;
	};
	if(NumOfParams%2==1)
	{
	val1=calculateVariable(getParameter(parameters,NumOfParams-1,FALSE, function)).toDouble(&ok);
	if(val1<val2) val2=val1;
	};
	return QString::number(val2);
   };
   if (function=="MAX")
   {
	//NumOfParams
	val2=0.0;
	for(w1=1;w1<=(NumOfParams/2);w1++)
	{
	val1=functionMax(getParameter(parameters, (w1-1)*2, FALSE, function), getParameter(parameters, (w1-1)*2+1, TRUE, function));
	val2=val1;
	};
	if(NumOfParams%2==1)
	{
	val1=calculateVariable(getParameter(parameters,NumOfParams-1,FALSE, function)).toDouble(&ok);
	if(val1>val2) val2=val1;
	};
	return QString::number(val2);
   };
   if (function=="AVERAGE")
   {
	val1=functionAvg(getParameter(parameters, 0, TRUE, function), getParameter(parameters, 1, TRUE, function));
	return QString::number(val1);
   };

   if(function=="BESSELI")
   {
	// BesselI (x,n)
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	vali=calculateVariable(getParameter(parameters, 1, TRUE, function)).toInt(&ok);
	val2=BesselI(vali,val1);
	return QString::number(val2);
   };
   if(function=="BESSELJ")
   {
	// BesselJ (x,n)
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	vali=calculateVariable(getParameter(parameters, 1, TRUE, function)).toInt(&ok);
	val2=BesselJ(vali,val1);
	return QString::number(val2);
   };
   if(function=="BESSELK")
   {
	// BesselK (x,n)
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	vali=calculateVariable(getParameter(parameters, 1, TRUE, function)).toInt(&ok);
	val2=BesselK(vali,val1);
	return QString::number(val2);
   };
   if(function=="BESSELY")
   {
	// BesselY (x,n)
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	vali=calculateVariable(getParameter(parameters, 1, TRUE, function)).toInt(&ok);
	val2=BesselY(vali,val1);
	return QString::number(val2);
   };
   if(function=="GAMMALN")
   {
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	val2=GammaLn(val1);
	return QString::number(val2);
   };
   if(function=="ERF")
   {
	// ERF (a,b)
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	val2=calculateVariable(getParameter(parameters, 1, TRUE, function)).toDouble(&ok);
	return QString::number(ErrorFunction(val2)-ErrorFunction(val1));
   };
   if(function=="ERFC")
   {
	// ERFC (a,b)
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	val2=calculateVariable(getParameter(parameters, 1, TRUE, function)).toDouble(&ok);
	return QString::number(ErrorFunctionComplementary(val2)-ErrorFunctionComplementary(val1));
   };
   if(function=="POISSON")
   {
	// POISSON DISTR(x,n,distr/desnt)
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	val2=calculateVariable(getParameter(parameters, 1, TRUE, function)).toDouble(&ok);
	vali=calculateVariable(getParameter(parameters, 2, TRUE, function)).toInt(&ok);
	if(vali==1)
	{
		return QString::number(GammaQ(floor(val1)+1, val2));
	}else
	{
		return QString::number(exp(-val2)*pow(val2,val1)/exp(GammaLn(val1+1.0)));
	};
   };
   if(function=="CHIDIST")
   {
	// POISSON CHIDIST(x,n,distr/density)
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	val2=calculateVariable(getParameter(parameters, 1, TRUE, function)).toDouble(&ok);
	vali=calculateVariable(getParameter(parameters, 2, TRUE, function)).toInt(&ok);
	if(vali==1)
	{
		return QString::number(GammaP(val2/2.0,val1*val1/2.0));
	} else
	{
		return QString::number(
			pow(val1,val2-1.0)*exp(-val1*val1/2)/ ( pow(2,val2/2.0-1.0)*exp(GammaLn(val2/2.0)))
		);
	};
   };
   if(function=="CHI2DIST")
   {
	// POISSON CHISQUAREDIST(x,n,distr/density)
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	val2=calculateVariable(getParameter(parameters, 1, TRUE, function)).toDouble(&ok);
	vali=calculateVariable(getParameter(parameters, 2, TRUE, function)).toInt(&ok);
	if(vali==1)
	{
		return QString::number(GammaP(val2/2.0,val1/2.0));
	} else
	{
		return QString::number(
			pow(val1,val2/2.0-1.0)/(exp(val1/2.0)*pow(sqrt(2.0),val2)*exp(GammaLn(val2/2.0)))
		);
	};
   };
   if(function=="BETAI")
   {
	// BETA INCOMPLETE BETA(x,a,b)
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	val2=calculateVariable(getParameter(parameters, 1, TRUE, function)).toDouble(&ok);
	val3=calculateVariable(getParameter(parameters, 2, TRUE, function)).toDouble(&ok);
	return QString::number(BetaIncomplete(val2,val3,val1));
   };
   if(function=="GAMMAP")
   {
	// GammaP (x,a)
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	val2=calculateVariable(getParameter(parameters, 1, TRUE, function)).toDouble(&ok);
	return QString::number(GammaP(val2,val1));
   };
   if(function=="GAMMAQ")
   {
	// GammaQ (x,a)
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	val2=calculateVariable(getParameter(parameters, 1, TRUE, function)).toDouble(&ok);
	return QString::number(GammaQ(val2,val1));
   };
   if (function=="VAR")
   {
	val1=functionVariance(getParameter(parameters, 0, TRUE, function), getParameter(parameters, 1, TRUE, function));
	return QString::number(val1);
   };
   if (function=="VARP")
   {
	val1=functionVariancePopulation(getParameter(parameters, 0, TRUE, function), getParameter(parameters, 1, TRUE, function));
	return QString::number(val1);
   };
   if (function=="STDEV")
   {
	val1=functionVariance(getParameter(parameters, 0, TRUE, function), getParameter(parameters, 1, TRUE, function));
	if(val1<=0.0) return QString::number(0.0);
	return QString::number(sqrt(val1));
   };
   if (function=="STDEVP")
   {
	val1=functionVariancePopulation(getParameter(parameters, 0, TRUE, function), getParameter(parameters, 1, TRUE, function));
	if(val1<=0.0) return QString::number(0.0);
	return QString::number(sqrt(val1));
   };
   if (function=="SKEW")
   {
	val1=functionSkew(getParameter(parameters, 0, TRUE, function), getParameter(parameters, 1, TRUE, function));
	return QString::number(val1);
   };
   if (function=="KURT")
   {
	val1=functionKurt(getParameter(parameters, 0, TRUE, function), getParameter(parameters, 1, TRUE, function));
	return QString::number(val1);
   };
   if(function=="GAMMADIST")
   {
	// GAMMADIST (x,alpha,beta,distribution?density1:0)
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	val2=calculateVariable(getParameter(parameters, 1, TRUE, function)).toDouble(&ok);
	val3=calculateVariable(getParameter(parameters, 2, TRUE, function)).toDouble(&ok);
	vali=calculateVariable(getParameter(parameters, 3, TRUE, function)).toInt(&ok);
	if(vali==1)//distribution
	{
		if(val3==0.0) return QString::number(0.0);
		else
			return QString::number(GammaP(val2,val1/val3));
	}else //density
	{
		return QString::number(
			pow(val1,val2-1.0)*exp(-val1/val3) / (pow(val3,val2)*exp(GammaLn(val2)))
			);
	};
   };
   if(function=="BETADIST")
   {
	// BETADIST (z,alpha,beta,distribution?density1:0)
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	val2=calculateVariable(getParameter(parameters, 1, TRUE, function)).toDouble(&ok);
	val3=calculateVariable(getParameter(parameters, 2, TRUE, function)).toDouble(&ok);
	vali=calculateVariable(getParameter(parameters, 3, TRUE, function)).toInt(&ok);
	if(vali==1)//distribution
	{
		return QString::number(BetaIncomplete(val2,val3,val1));
	}else //density
	{
		return QString::number(
			pow(val1,val2-1.0)*pow(1.0-val1,val3-1.0) / Beta(val2,val3)
			);
	};
   };
   if(function=="FDIST")
   {
	// FDIST (z,d1,d2,distribution?density1:0)
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	val2=calculateVariable(getParameter(parameters, 1, TRUE, function)).toDouble(&ok);
	val3=calculateVariable(getParameter(parameters, 2, TRUE, function)).toDouble(&ok);
	vali=calculateVariable(getParameter(parameters, 3, TRUE, function)).toInt(&ok);
	if(vali==1)//distribution
	{
		return QString::number(
			-BetaIncomplete(val3/2,val2/2,val3/(val3+val2*val1))
				+BetaIncomplete(val3/2,val2/2,1)
			);
	}else //density
	{
		return QString::number(
			pow(val2,val2/2)*pow(val3,val3/2)*pow(val1,val2/2-1)/
				(pow(val3+val2*val1,(val2+val3)/2)*Beta(val2/2,val3/2))
			);
	};
   };
   if(function=="NORMALDIST")
   {
	// NORMALDIST (x,m,s,distribution?density1:0)
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	val2=calculateVariable(getParameter(parameters, 1, TRUE, function)).toDouble(&ok);
	val3=calculateVariable(getParameter(parameters, 2, TRUE, function)).toDouble(&ok);
	vali=calculateVariable(getParameter(parameters, 3, TRUE, function)).toInt(&ok);
	if(vali==1)//distribution
	{
		return QString::number(
			(ErrorFunction((val1-val2)/(sqrt(2)*val3))+1)/2.0
			);
	}else //density
	{
		return QString::number(
			exp(-pow(((val1-val2)/val3),2)/2)/(val3*sqrt(2*M_PI))
			);
	};
   };
   if(function=="PHI")
   {
	// NORMALDIST (x,distribution?density1:0) with mean=0 s=1.0
	val1=calculateVariable(getParameter(parameters, 0, TRUE, function)).toDouble(&ok);
	vali=calculateVariable(getParameter(parameters, 1, TRUE, function)).toInt(&ok);
	if(vali==1)//distribution
	{
		return QString::number(
			(ErrorFunction(val1/(sqrt(2)))+1)/2.0
			);
	}else //density
	{
		return QString::number(
			exp(-pow(val1,2)/2)/(sqrt(2*M_PI))
			);
	};
   };
	/*
	StudentTDistribution/: PDF[StudentTDistribution[n_], x_] :=
      	1/(Sqrt[n] Beta[n/2, 1/2]) Sqrt[n/(n+x^2)]^(n+1) /;
					ParameterQ[StudentTDistribution[n]]

	StudentTDistribution/: CDF[StudentTDistribution[n_], x_] :=
      	(1 + Sign[x] BetaRegularized[n/(n+x^2), 1, n/2, 1/2])/2 /;
					ParameterQ[StudentTDistribution[n]]
	*/



  return 0;
};




QString Sheet::dataParserHelper(const QString &data)
{
	if(data.left(1)=="""" && data.right(1)=="""") return QString(data);
	Expression exp1(data);
	exp1.Parse();
	QStack<QString> stack1;
	stack1.setAutoDelete(TRUE);
	int i=0;
	QString* s1;
	QString* s2=NULL;
	int* i1;
	int args,tokentype;
	QString tempval;
	s1=exp1.CompiledBody.first();i1=exp1.CompiledBodyType.first();
	while(i<=(int)exp1.CompiledBody.count()-1)
	{
		args= ((*i1) & 0xFF00)>>8; tokentype=(*i1) & 0x00FF;
		if(tokentype==NUMBER_TOKEN)
		{
			stack1.push(new QString(*s1));
			//printf("Parse:Number=%s\r\n",s1->latin1());
		}
		else if(tokentype==VARIABLE_TOKEN)
		{
			stack1.push(new QString(QString(*s1).upper()));
			//printf("Parse:Var=%s\r\n",s1->latin1());
			//here to put implementation of other types of variables except cell.
			//for example names
		}
		else if(tokentype==STRING_TOKEN)
		{
			stack1.push(new QString(*s1));
			//printf("Parse:String=%s\r\n",s1->ascii());
		}
		else if(tokentype==FUNCTION_TOKEN)
		{
			QString params="";
			for(int w1=1;w1<=args;w1++)
			{
				if((int)stack1.count()!=0) s2=stack1.pop();
				params=*s2+params;//args in reverse order
				params=","+params;
			};
				params=params.mid(1);
				if(params==NULL) params="0";
				//printf("Parse:Func=%s, params=%s, stackcount=%d,args=%d\r\n"
				//	,s1->latin1(),params.latin1(),stack1.count(),args);
				tempval=calculateFunction(*s1,params,args);
				tempval=tempval;
				stack1.push(new QString(tempval));
		};

		//loops to next token
		if(exp1.CompiledBody.next()!=NULL) s1=exp1.CompiledBody.current(); else break;
		if(exp1.CompiledBodyType.next()!=NULL) i1=exp1.CompiledBodyType.current(); else break;
		i++;
	};
	if((int)stack1.count()!=0)s2=stack1.pop(); else s2=new QString("!ERROR");
	tempval=*s2;
	return(tempval);
};



QString Sheet::dataParser(const QString &cell, const QString &data)
{
  QString strippedData(data);
  strippedData.replace(QRegExp("\\s"), "");
  if (strippedData.isEmpty() || strippedData[0]!='=') return data;
  if (listDataParser.find(cell)!=listDataParser.end()) return "0";
  listDataParser.append(cell);
 // printf("DATAPARSER: data=%s, cell=%s\r\n",data.ascii(),cell.ascii());
  strippedData=dataParserHelper(strippedData.remove(0, 1).replace(QRegExp(":"), ","));

  int i=0;
  QString tempParameter(getParameter(strippedData, i)), result="";
  do
  {
    result+=","+calculateVariable(tempParameter);
    tempParameter=getParameter(strippedData, ++i);
  }
  while (!tempParameter.isNull());
  listDataParser.remove(cell);
  return result.mid(1);
}


void Sheet::setData(const QString &data)
{
  setText(currentRow(), currentColumn(), data);
  slotCellChanged(currentRow(), currentColumn());
  activateNextCell();
}

QString Sheet::getData()
{
  typeCellData *cellData=findCellData(currentRow(), currentColumn());
  if (cellData)
    return cellData->data;
  return "";
}

void Sheet::lockClicks(bool lock)
{
  clicksLocked=lock;
}

void Sheet::paintCell(QPainter *p, int row, int col, const QRect & cr, bool selected)
{
  if (selected && row==currentRow() && col==currentColumn()) selected=FALSE;

  int sheetDataCurrent=sheetData.at();
  typeCellData *cellData=findCellData(row, col);
  if (sheetDataCurrent>=0) sheetData.at(sheetDataCurrent);
  if (!cellData) cellData=&defaultCellData;
  if (selected)
    p->fillRect(0, 0, cr.width(), cr.height(), colorGroup().highlight());
  else
  {
    p->fillRect(0, 0, cr.width(), cr.height(), colorGroup().base());
    p->fillRect(0, 0, cr.width(), cr.height(), cellData->background);
  }

  QTableItem *cellItem=item(row, col);
  if (cellItem)
  {
    p->setPen(selected ? colorGroup().highlightedText() : cellData->fontColor);
    p->setFont(cellData->font);
    QString str=cellItem->text();
    p->drawText(2, 2, cr.width()-4, cr.height()-4, cellData->alignment, cellItem->text());
  }

  int rx=cr.width()-1, ry=cr.height()-1;
  QPen pen(p->pen());
  p->setPen(cellData->borders.right);
  p->drawLine(rx, 0, rx, ry);
  p->setPen(cellData->borders.bottom);
  p->drawLine(0, ry, rx, ry);
  p->setPen(pen);
}

void Sheet::viewportMousePressEvent(QMouseEvent *e)
{
  QMouseEvent ce(e->type(), viewportToContents(e->pos()), e->globalPos(), e->button(), e->state());
  if (clicksLocked)
  {
    if (selectionNo<0)
    {
      clearSelection();
      QTableSelection newSelection;
      newSelection.init(rowAt(ce.pos().y()), columnAt(ce.pos().x()));
      newSelection.expandTo(newSelection.anchorRow(), newSelection.anchorCol());
      selectionNo=addSelection(newSelection);
    }
  }
  else
    QTable::contentsMousePressEvent(&ce);
}

void Sheet::viewportMouseMoveEvent(QMouseEvent *e)
{
  QMouseEvent ce(e->type(), viewportToContents(e->pos()), e->globalPos(), e->button(), e->state());
  if (clicksLocked)
  {
    if (selectionNo>=0)
    {
      QTableSelection oldSelection(selection(selectionNo));
      oldSelection.expandTo(rowAt(ce.pos().y()), columnAt(ce.pos().x()));
      if (!(oldSelection==selection(selectionNo)))
      {
        removeSelection(selectionNo);
        selectionNo=addSelection(oldSelection);
      }
    }
  }
  else
    QTable::contentsMouseMoveEvent(&ce);
}

void Sheet::viewportMouseReleaseEvent(QMouseEvent *e)
{
  QMouseEvent ce(e->type(), viewportToContents(e->pos()), e->globalPos(), e->button(), e->state());
  if (clicksLocked && selectionNo>=0)
  {
    QTableSelection oldSelection(selection(selectionNo));
    oldSelection.expandTo(rowAt(ce.pos().y()), columnAt(ce.pos().x()));
    removeSelection(selectionNo);
    selectionNo=-1;
    if (oldSelection.topRow()==oldSelection.bottomRow() && oldSelection.leftCol()==oldSelection.rightCol())
      emit cellClicked(findCellName(oldSelection.topRow(), oldSelection.leftCol()));
    else
      emit cellClicked(findCellName(oldSelection.topRow(), oldSelection.leftCol())+','+findCellName(oldSelection.bottomRow(), oldSelection.rightCol()));
  }
  else
    QTable::contentsMouseReleaseEvent(&ce);
}

QString Sheet::findCellName(int row, int col)
{
  return (getHeaderString(col+1)+QString::number(row+1));
}

void Sheet::copySheetData(QList<typeCellData> *destSheetData)
{
  typeCellData *tempCellData, *newCellData;
  destSheetData->clear();

  for (tempCellData=sheetData.first(); tempCellData; tempCellData=sheetData.next())
  {
    newCellData=new typeCellData;
    *newCellData=*tempCellData;
    destSheetData->append(newCellData);
  }
}

void Sheet::setSheetData(QList<typeCellData> *srcSheetData)
{
  typeCellData *tempCellData, *newCellData;

  for (tempCellData=sheetData.first(); tempCellData; tempCellData=sheetData.next())
  {
    clearCell(tempCellData->row, tempCellData->col);
    updateCell(tempCellData->row, tempCellData->col);
  }
  sheetData.clear();

  for (tempCellData=srcSheetData->first(); tempCellData; tempCellData=srcSheetData->next())
  {
    newCellData=new typeCellData;
    *newCellData=*tempCellData;
    sheetData.append(newCellData);
    setText(newCellData->row, newCellData->col, dataParser(findCellName(newCellData->row, newCellData->col), newCellData->data));
  }
  emit sheetModified();
}

void Sheet::setName(const QString &name)
{
  sheetName=name;
  emit sheetModified();
}

QString Sheet::getName()
{
  return sheetName;
}

void Sheet::setBrush(int row, int col, const QBrush &brush)
{
  typeCellData *cellData=findCellData(row, col);
  if (!cellData) cellData=createCellData(row, col);
  if (cellData)
  {
    cellData->background=brush;
    emit sheetModified();
  }
}

QBrush Sheet::getBrush(int row, int col)
{
  typeCellData *cellData=findCellData(row, col);
  if (!cellData) cellData=&defaultCellData;
  return cellData->background;
}

void Sheet::setTextAlign(int row, int col, Qt::AlignmentFlags flags)
{
  typeCellData *cellData=findCellData(row, col);
  if (!cellData) cellData=createCellData(row, col);
  if (cellData)
  {
    cellData->alignment=flags;
    emit sheetModified();
  }
}

Qt::AlignmentFlags Sheet::getAlignment(int row, int col)
{
  typeCellData *cellData=findCellData(row, col);
  if (!cellData) cellData=&defaultCellData;
  return cellData->alignment;
}

void Sheet::setTextFont(int row, int col, const QFont &font, const QColor &color)
{
  typeCellData *cellData=findCellData(row, col);
  if (!cellData) cellData=createCellData(row, col);
  if (cellData)
  {
    cellData->font=font;
    cellData->fontColor=color;
    emit sheetModified();
  }
}

QFont Sheet::getFont(int row, int col)
{
  typeCellData *cellData=findCellData(row, col);
  if (!cellData) cellData=&defaultCellData;
  return cellData->font;
}

QColor Sheet::getFontColor(int row, int col)
{
  typeCellData *cellData=findCellData(row, col);
  if (!cellData) cellData=&defaultCellData;
  return cellData->fontColor;
}

void Sheet::setPen(int row, int col, int vertical, const QPen &pen)
{
  typeCellData *cellData=findCellData(row, col);
  if (!cellData) cellData=createCellData(row, col);
  if (cellData)
  {
    if (vertical)
      cellData->borders.right=pen;
    else
      cellData->borders.bottom=pen;
    emit sheetModified();
  }
}

QPen Sheet::getPen(int row, int col, int vertical)
{
  typeCellData *cellData=findCellData(row, col);
  if (!cellData) cellData=&defaultCellData;
  return (vertical ? cellData->borders.right : cellData->borders.bottom);
}

void Sheet::getSelection(int *row1, int *col1, int *row2, int *col2)
{
  int selectionNo=currentSelection();
  if (selectionNo>=0)
  {
    QTableSelection selection(selection(selectionNo));
    *row1=selection.topRow();
    *row2=selection.bottomRow();
    *col1=selection.leftCol();
    *col2=selection.rightCol();
  }
  else
  {
    *row1=*row2=currentRow();
    *col1=*col2=currentColumn();
  }
}

void Sheet::editClear()
{
  int row1, row2, col1, col2;
  getSelection(&row1, &col1, &row2, &col2);

  int row, col;
  for (row=row1; row<=row2; ++row)
    for (col=col1; col<=col2; ++col)
    {
      setText(row, col, "");
      slotCellChanged(row, col);
    }
}

void Sheet::editCopy()
{
  clipboardData.clear();

  int row1, row2, col1, col2;
  getSelection(&row1, &col1, &row2, &col2);

  typeCellData *cellData, *newCellData;
  int row, col;
  for (row=row1; row<=row2; ++row)
    for (col=col1; col<=col2; ++col)
    {
      cellData=findCellData(row, col);
      if (cellData)
      {
        newCellData=new typeCellData;
        *newCellData=*cellData;
        newCellData->row-=row1;
        newCellData->col-=col1;
        clipboardData.append(newCellData);
      }
    }
}

void Sheet::editCut()
{
  editCopy();
  editClear();
}

void Sheet::editPaste(bool onlyContents)
{
  int row1=currentRow(), col1=currentColumn();
  typeCellData *cellData, *tempCellData;

  for (tempCellData=clipboardData.first(); tempCellData; tempCellData=clipboardData.next())
  {
    cellData=findCellData(tempCellData->row+row1, tempCellData->col+col1);
    if (!cellData) cellData=createCellData(tempCellData->row+row1, tempCellData->col+col1);
    if (cellData)
    {
      if (onlyContents)
        cellData->data=tempCellData->data;
      else
      {
        *cellData=*tempCellData;
        cellData->row+=row1;
        cellData->col+=col1;
      }
      setText(cellData->row, cellData->col, dataParser(findCellName(cellData->row, cellData->col), cellData->data));
      emit sheetModified();
    }
  }
}

void Sheet::insertRows(int no, bool allColumns)
{
  setNumRows(numRows()+no);

  typeCellData *tempCellData;
  int row=currentRow(), col=currentColumn();

  for (tempCellData=sheetData.first(); tempCellData; tempCellData=sheetData.next())
    if (tempCellData->row>=row && (allColumns || tempCellData->col==col))
    {
      clearCell(tempCellData->row, tempCellData->col);
      tempCellData->row+=no;
    }
  for (tempCellData=sheetData.first(); tempCellData; tempCellData=sheetData.next())
    if (tempCellData->row>=row && (allColumns || tempCellData->col==col))
    {
      updateCell(tempCellData->row-no, tempCellData->col);
      setText(tempCellData->row, tempCellData->col, dataParser(findCellName(tempCellData->row, tempCellData->col), tempCellData->data));
    }
 emit sheetModified();
}

void Sheet::insertColumns(int no, bool allRows)
{
  int noCols=numCols();
  int newCols=noCols+no;
  setNumCols(newCols);
  for (int i=noCols; i<newCols; ++i)
    horizontalHeader()->setLabel(i, getHeaderString(i+1), DEFAULT_COL_WIDTH);

  typeCellData *tempCellData;
  int col=currentColumn(), row=currentRow();

  for (tempCellData=sheetData.first(); tempCellData; tempCellData=sheetData.next())
    if (tempCellData->col>=col && (allRows || tempCellData->row==row))
    {
      clearCell(tempCellData->row, tempCellData->col);
      tempCellData->col+=no;
    }
  for (tempCellData=sheetData.first(); tempCellData; tempCellData=sheetData.next())
    if (tempCellData->col>=col && (allRows || tempCellData->row==row))
    {
      updateCell(tempCellData->row, tempCellData->col-no);
      setText(tempCellData->row, tempCellData->col, dataParser(findCellName(tempCellData->row, tempCellData->col), tempCellData->data));
    }
  emit sheetModified();
}

void Sheet::dataFindReplace(const QString &findStr, const QString &replaceStr, bool matchCase, bool allCells, bool entireCell, bool replace, bool replaceAll)
{
  typeCellData *tempCellData;
  int row1, col1, row2, col2;
  getSelection(&row1, &col1, &row2, &col2);
  bool found=FALSE;

  for (tempCellData=sheetData.first(); tempCellData; tempCellData=sheetData.next())
    if (allCells || (tempCellData->row>=row1 && tempCellData->row<=row2 && tempCellData->col>=col1 && tempCellData->col<=col2))
    {
      QTableItem *cellItem=item(tempCellData->row, tempCellData->col);
      if (cellItem && (entireCell ? (matchCase ? cellItem->text()==findStr : cellItem->text().upper()==findStr.upper()) : cellItem->text().find(findStr, 0, matchCase)>=0))
      {
        if (!found)
        {
          found=TRUE;
          clearSelection();
        }
        setCurrentCell(tempCellData->row, tempCellData->col);
        if (replace)
        {
          tempCellData->data=cellItem->text().replace(QRegExp(findStr, matchCase), replaceStr);
          setText(tempCellData->row, tempCellData->col, dataParser(findCellName(tempCellData->row, tempCellData->col), tempCellData->data));
        }
        if (!replace || !replaceAll) break;
      }
    }

 if (found)
 {
   if (replace)
     slotCellChanged(currentRow(), currentColumn());
 }
 else
   QMessageBox::warning(this, tr("Error"), tr("Search key not found!"));
}

//
// Static functions
//

QString Sheet::getHeaderString(int section)
{
  if (section<1) return "";
  return getHeaderString((section-1)/26)+QChar('A'+(section-1)%26);
}

int Sheet::getHeaderColumn(const QString &section)
{
  if (section.isEmpty()) return 0;
  return (section[section.length()-1]-'A'+1)+getHeaderColumn(section.left(section.length()-1))*26;
}


//Expression Parser Class Definition


QChar Expression::chunk0(void)
{
	if(chunk.length()>0) return(chunk[0]); else return('\0');
};

Expression::Expression(QString expr1)// constructor
{
	Body=expr1;
	SYMBOL="+-*/%^=()<>&|!,";
	MATHSYMBOL="+-*/%^=<>&|!,";
	//		lnlim=1.0e-36;      // Smallest number allowed
	//		loglim=1.0e-10 ;    // Smallest number allowed in call to log10() *
	ErrorFound=TRUE;
	n=0;chunk="";SymbGroup=NONE_TOKEN;InExpr=Body;
	ArgsOfFunc=0;
	CompiledBody.setAutoDelete(TRUE);
	CompiledBodyType.setAutoDelete(TRUE);
	//CompiledBody=QStringList(0);
};

bool Expression::isSymbol(QChar ch)
{
	int j = 0;
	while (j<=((int)SYMBOL.length()-1) && ch!=SYMBOL[j]) j++;
	if(j<((int)SYMBOL.length())) return true; else return false;
};

bool Expression::isMathSymbol(QChar ch)
{
	int j = 0;
	while (j<=((int)MATHSYMBOL.length()-1) && ch!=MATHSYMBOL[j]) j++;
	if(j<((int)MATHSYMBOL.length())) return true; else return false;
};

void Expression::GetNext()
{
	chunk="";
	if(n>=(int)InExpr.length()) return;
	while (InExpr[n]==' ') n++;
	if(InExpr[n]=='\"')
	{
		while ( (n<(int)InExpr.length()) && (InExpr[n+1]!='\"') )
		{
			printf("chunk=%s\r\n",chunk.latin1());
			chunk+=InExpr[n];
			n++;
		};
		chunk+=InExpr[n];
		printf("2\r\n");
		SymbGroup=STRING_TOKEN;
	}
	else if (isSymbol(InExpr[n]))
	{
		SymbGroup=SYMBOL_TOKEN;
		chunk+=InExpr[n];
		n++;
		if( (n<(int)InExpr.length()) &&
			isMathSymbol(InExpr[n-1])  &&
				isMathSymbol(InExpr[n])		 )
		{
			SymbGroup=SYMBOL_TOKEN;
			chunk+=InExpr[n];
			n++;
		};
	}
	else if ((InExpr[n].isLetter())||(InExpr[n]=='#'))
	{
		while ( (n<(int)InExpr.length()) && !isSymbol(InExpr[n]) )
		{
			if (!(InExpr[n]==' ')) chunk+=InExpr[n];
			n++;
		};
		if (InExpr[n]=='(') SymbGroup=FUNCTION_TOKEN; // function TOKEN
						else SymbGroup=VARIABLE_TOKEN;
	}
	else if((n<(int)InExpr.length()) &&
			((InExpr[n].isDigit()) || (InExpr[n]=='.')))
	{
		while( n<(int)InExpr.length() )
		{
			if((InExpr[n].isDigit()) || InExpr[n]=='.')
			{
				chunk+=InExpr[n];
				SymbGroup=NUMBER_TOKEN;
				n++;
			}
			else if(InExpr[n]=='e')
			{
				if((n+1)<(int)InExpr.length())
				{
					if(InExpr[n+1]=='-' || InExpr[n+1]=='+' ||  InExpr[n+1].isDigit())
					{
						chunk+=InExpr[n];
						chunk+=InExpr[n+1];
						SymbGroup=NUMBER_TOKEN;
						n+=2;
					}
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}//while
	}//else if
};//end function


void Expression::First()
{
	GetNext();
	if (!(chunk=="") && !ErrorFound) Third();
	else ErrorFound = true;
};

void Expression::Third()
{
	QChar sign, secS='\0';
	Fourth();
	sign = chunk0();
	if((int)chunk.length()>1) secS=chunk[1];
	while( sign == '+' || sign == '-'||
		sign == '<' || sign == '>'|| sign == '%'||
		sign == '&' || sign == '|' || sign == '!' || sign == '='
		)
	{
		GetNext();
		Fourth();
		QString name;
		if( sign == '+' ) name= "+" ;
		else if(sign=='-') name= "-" ;
		else if(sign=='>' && secS=='\0') name= ">" ;
		else if(sign=='<' && secS=='\0') name= "<" ;
		else if(sign=='=' && secS=='=') name= "==" ;
		else if(sign=='!' && secS=='=') name= "!=" ;
		else if(sign=='>' && secS=='=') name= ">=" ;
		else if(sign=='<' && secS=='=') name= "<=" ;
		else if(sign=='&' && secS=='&') name= "AND" ;
		else if(sign=='|' && secS=='|') name= "OR" ;
		else if(sign=='%') name= "MOD" ;
		CompiledBody.append(new QString(name)); // not sure if pushed in the back.
		CompiledBodyType.append(new int(FUNCTION_TOKEN | 2<<8)); //2 argument functions
		sign = chunk0();
	}
};

void Expression::Fourth()
{
	QChar sign;
	Fifth();
	sign = chunk0();
	while( sign == '*' || sign == '/' )
	{
		GetNext();
		Fifth();
		QString name;
		if( sign == '*' ) name= "*" ;
		else name= "/" ;
		CompiledBody.append(new QString(name));
		CompiledBodyType.append(new int(FUNCTION_TOKEN | 2<<8)); //2 arguments functions
		sign = chunk0();
	}
};

void Expression::Fifth()
{
	Sixth();
	//if(chunk.Length==0) return;
	if( chunk0() == '^' )
	{
		GetNext();
		Fifth();
		CompiledBody.append(new QString("POWER"));
		CompiledBodyType.append(new int(FUNCTION_TOKEN | 2<<8)); // 2 argument functions
	}
};

void Expression::Sixth()
{
	char sign;
	sign = ' ';
	if(SymbGroup== SYMBOL_TOKEN &&
		chunk0() == '+' || chunk0() == '-'  | chunk0() == '!')
	{
		sign = chunk0();
		GetNext();
	}
	Seventh();
	if( sign == '-' )
	{
		CompiledBody.append(new QString("CHGSGN")); // unary minus
		CompiledBodyType.append(new int(FUNCTION_TOKEN | 1<<8)); //1 argument
	}
	if( sign == '!' )
	{
		CompiledBody.append(new QString("NOT")); // unary minus
		CompiledBodyType.append(new int(FUNCTION_TOKEN | 1<<8)); //1 argument
	}
};


void Expression::Seventh()
{
	if( chunk0() == '(' && SymbGroup==SYMBOL_TOKEN)
	{
		GetNext();
		Third(); //parse the insides until we get a ')'
		if (chunk0() != ')') ErrorFound = true;
		GetNext();
	}
	else Eighth();
};

void Expression::Eighth()
{
	if ( SymbGroup== NUMBER_TOKEN )
	{
		CompiledBody.append(new QString(chunk));
		CompiledBodyType.append(new int(NUMBER_TOKEN));
		GetNext();
	}
	else if ( SymbGroup== VARIABLE_TOKEN  )
	{
		CompiledBody.append(new QString(chunk));
		CompiledBodyType.append(new int(VARIABLE_TOKEN));
		GetNext();
	}
	else if (SymbGroup== STRING_TOKEN )
	{
		CompiledBody.append(new QString(chunk+QString("\"")));
		CompiledBodyType.append(new int(STRING_TOKEN));
		GetNext();
	}
	else Ninth();
};

void Expression::Ninth()
{
	if (  SymbGroup== FUNCTION_TOKEN  )
	{
		QString TempFunk =  chunk ;
		GetNext();
		if(chunk0() == '(' )
		{
			FuncDepth++;
			ArgsOfFunc.resize(FuncDepth+1);
			ArgsOfFunc[FuncDepth]=1;
			//ArgsOfFunc=1;
			GetNext();
			Third();
			while(chunk0()==',')  //function arguments separator
			{
				//ArgsOfFunc++;
				ArgsOfFunc[FuncDepth]++;
				GetNext();
				Third();
			};
			if (chunk0() != ')') ErrorFound = true;

			CompiledBody.append(new QString(TempFunk));
			if (TempFunk=="PI") ArgsOfFunc[FuncDepth]=0;
			// couldn't find a better way to parse function PI() with 0 args. :)
			CompiledBodyType.append(new int(FUNCTION_TOKEN | (ArgsOfFunc[FuncDepth]<<8) ));
			//the mask &FF00 gives the arguments of the functions passed.
			FuncDepth--;
			ArgsOfFunc.resize(FuncDepth+1);
			GetNext();
		}
	}
	else
	ErrorFound = true;
};

bool Expression::Parse()
{
	CompiledBody.clear();
	ErrorFound = false;
	n = 0;ArgsOfFunc.resize(0);FuncDepth=0;
	InExpr=Body;
	First();
	return(!ErrorFound);
};


