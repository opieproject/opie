

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/types.h>
#include <strings.h>
#include <qstring.h>
#include <qlist.h>
#include <qarray.h>
#include "Excel.h"

static xfrecord formatter[] = {
{ 0xe , DATEFORMAT, "%m/%d/%y"},
{ 0xf , DATEFORMAT, "%d-%b-%y"},
{ 0x10, DATEFORMAT, "%d-%b"},
{ 0x11, DATEFORMAT, "%b-%y"},
{ 0x12, DATEFORMAT, "%I:%M %p"},
{ 0x13, DATEFORMAT, "%I:%M:%S %p"},
{ 0x14, DATEFORMAT, "%H:%M"},
{ 0x15, DATEFORMAT, "%H:%M:%S"},
{ 0x16, DATEFORMAT, "%m/%d/%y %H:%M"},
{ 0x2d, DATEFORMAT, "%M:%S"},
{ 0x2e, DATEFORMAT, "%H:%M:%S"},
{ 0x2f, DATEFORMAT, "%M:%S"},
{ 0xa5, DATEFORMAT, "%m/%d/%y %I:%M %p"},
{ 0x1 , NUMBERFORMAT, "%.0f"},
{ 0x2 , NUMBERFORMAT, "%.2f"},
{ 0x3 , NUMBERFORMAT, "#,##%.0f"},
{ 0x4 , NUMBERFORMAT, "#,##%.2f"},
{ 0x5 , NUMBERFORMAT, "$#,##%.0f"},
{ 0x6 , NUMBERFORMAT, "$#,##%.0f"},
{ 0x7 , NUMBERFORMAT, "$#,##%.2f"},
{ 0x8 , NUMBERFORMAT, "$#,##%.2f"},
{ 0x9 , NUMBERFORMAT, "%.0f%%"},
{ 0xa , NUMBERFORMAT, "%.2f%%"},
{ 0xb , NUMBERFORMAT, "%e"},
{ 0x25, NUMBERFORMAT, "#,##%.0f;(#,##0)"},
{ 0x26, NUMBERFORMAT, "#,##%.0f;(#,##0)"},
{ 0x27, NUMBERFORMAT, "#,##%.2f;(#,##0.00)"},
{ 0x28, NUMBERFORMAT, "#,##%.2f;(#,##0.00)"},
{ 0x29, NUMBERFORMAT, "#,##%.0f;(#,##0)"},
{ 0x2a, NUMBERFORMAT, "$#,##%.0f;($#,##0)"},
{ 0x2b, NUMBERFORMAT, "#,##%.2f;(#,##0.00)"},
{ 0x2c, NUMBERFORMAT, "$#,##%.2f;($#,##0.00)"},
{ 0x30, NUMBERFORMAT, "##0.0E0"},
{ 0, 0, ""}
};



int ExcelBook::Integer2Byte(int b1, int b2)
{
	int i1 = b1 & 0xff;
	int i2 = b2 & 0xff;
	int val = i2 << 8 | i1;
	return val;
};

int ExcelBook::Integer4Byte(int b1,int b2,int b3,int b4)
{
	int i1 = Integer2Byte(b1, b2);
	int i2 = Integer2Byte(b3, b4);
	int val = i2 << 16 | i1;
	return val;
};

int ExcelBook::Integer2ByteFile(FILE *f) {
	int i1, i2;
	i1 = fgetc(f);
	i2 = fgetc(f);
	return Integer2Byte(i1,i2);
};

float ExcelBook::Float4Byte(int b1, int b2, int b3, int b4)
{
	int i;
	float f;
	unsigned char *ieee;
	ieee = (unsigned char *) &f;
	for (i = 0; i < 4; i++) ieee[i] = 0;
	ieee[0] = ((int)b4) & 0xff;
	ieee[1] = ((int)b3) & 0xff;
	ieee[2] = ((int)b2) & 0xff;
	ieee[3] = ((int)b1) & 0xff;
	return f;
};

double ExcelBook::Double4Byte(int b1, int b2, int b3, int b4)
{
	long int rk;
	double value;

	rk=Integer4Byte(b1,b2,b3,b4);
	//printf("Double4Bytes:%d,%d,%d,%d\r\n",b1,b2,b3,b4);
	if ( (rk & 0x02) != 0)
	{
		long int intval = rk >> 2; //drops the 2 bits
		printf("Double4Byte:intval=%d, rk=%d, rk>>2=%d\r\n",intval,rk,rk>>2);
		value = (double) intval;
		printf("Double4Byte: VALUEINT=%f\r\n",value);
		if ( (rk & 0x01) != 0)
		{
			value /= 100.0;
		};
		return value;
	}else
	{

		union { double d; unsigned long int b[2]; } dbl_byte;
		unsigned long int valbits = (rk & 0xfffffffc);
		#if defined(__arm__) && !defined(__vfp__)
		dbl_byte.b[0]=valbits;
		dbl_byte.b[1]=0;
		#else
		dbl_byte.b[0]=0;
		dbl_byte.b[1]=valbits;
		#endif
		printf("dbl_byte.b[0]=%d,dbl_byte.b[1]=%d\r\n",dbl_byte.b[0],dbl_byte.b[1]);
		value=dbl_byte.d;
		printf("Double4Byte: VALUE=%f\r\n",value);

		if ( (rk & 0x01) != 0)
      		{
        		value /= 100.0;
      		};
      		return value;
    	};
};

void ExcelBook::DetectEndian(void)
{
	int end;
	long i = 0x44332211;
	unsigned char* a = (unsigned char*) &i;
	end = (*a != 0x11);
	if (end == 1) {
		endian = BIG_ENDIAN;
		printf("BIGENDIAN!\r\n");
	} else {
		endian = LITTLE_ENDIAN;
		printf("LITTLEENDIAN!\r\n");
	}
};

double ExcelBook::Double8Byte(int b1, int b2, int b3, int b4, int b5, int b6, int b7, int b8)
{
	int i;
	double d;
	unsigned char *ieee;
	ieee = (unsigned char *)&d;
	for (i = 0; i < 8; i++) ieee[i] = 0;
	if (endian == BIG_ENDIAN) {
		ieee[0] = ((int)b8) & 0xff;ieee[1] = ((int)b7) & 0xff;
		ieee[2] = ((int)b6) & 0xff;ieee[3] = ((int)b5) & 0xff;
		ieee[4] = ((int)b4) & 0xff;ieee[5] = ((int)b3) & 0xff;
		ieee[6] = ((int)b2) & 0xff;ieee[7] = ((int)b1) & 0xff;
	} else {
		ieee[0] = ((int)b1) & 0xff;ieee[1] = ((int)b2) & 0xff;
		ieee[2] = ((int)b3) & 0xff;ieee[3] = ((int)b4) & 0xff;
		ieee[4] = ((int)b5) & 0xff;ieee[5] = ((int)b6) & 0xff;
		ieee[6] = ((int)b7) & 0xff;ieee[7] = ((int)b8) & 0xff;
	}
	return d;
};

bool ExcelBook::OpenFile(char *Filename)
{
	printf("Opening excel file!\r\n");
	File= fopen(Filename, "r");
	Position=0; // first byte index in file
	XFRecords.resize(0);
	SharedStrings.resize(0);
	Names.resize(0);
	Sheets.resize(0);
	if(File==NULL) return false;
	printf("Opened excel file!\r\n");
	return true;
};

bool ExcelBook::CloseFile(void)
{
	int w1;
	for(w1=0;w1<(int)XFRecords.count();w1++)
	{
		if(XFRecords[w1]!=NULL) {delete XFRecords[w1];XFRecords[w1]=NULL;};
	};
	for(w1=0;w1<(int)SharedStrings.count();w1++)
	{
		if(SharedStrings[w1]!=NULL) {delete SharedStrings[w1];SharedStrings[w1]=NULL;};
	};
	for(w1=0;w1<(int)Names.count();w1++)
	{
		if(Names[w1]!=NULL) {delete Names[w1];Names[w1]=NULL;};
	};
	for(w1=0;w1<(int)Sheets.count();w1++)
	{
		if(Sheets[w1]!=NULL) {delete Sheets[w1];Sheets[w1]=NULL;};
	};
	XFRecords.resize(0);
	SharedStrings.resize(0);
	Names.resize(0);
	Sheets.resize(0);
	fclose(File);
	printf("closed excel file!\r\n");
	if(File==NULL) return true;
	return false;
};

void ExcelBook::SeekPosition(int pos)
{
	if(!feof(File))
	{
	Position=pos;
	//printf("SeekPosition:Pos:%d\r\n",Position);
	fseek(File,pos,SEEK_SET);
	};
};

void ExcelBook::SeekSkip(int pos)
{
	if(!feof(File))
	{
	Position=Position+pos;
	//printf("SeekSkip:Pos:%d\r\n",Position);
	fseek(File, Position, SEEK_SET);
	};
};

int ExcelBook::FileEOF(void)
{
	if(File!=NULL) return(feof(File)); else return 0;
	//EOF is defined in stdlib as -1
};

int ExcelBook::Get2Bytes(void)
{
	int i1,i2;
	i1=0; i2=0;
	if (!feof(File))
	{
		i1=fgetc(File);
		Position++;
	};
	if (!feof(File))
	{
		i2=fgetc(File);
		Position++;
	};
	return Integer2Byte(i1,i2);
};

char* ExcelBook::Read(int pos, int length)
{
	int i;
	char *data;
	data= new char[length];
	SeekPosition(pos);
	for(i=0; i<length; i++)
	{
		if(!feof(File)) data[i]=fgetc(File);
	};
	Position= Position+length;
	return data;
};

QString ExcelBook::ReadUnicodeChar(int pos, int length)
{
	int i;
	QString data;
	int i1=' ',i2=' ',ii;
	SeekPosition(pos);
	for(i=0; i<length; i++)
	{
		if(!feof(File)) i1=fgetc(File);
		if(!feof(File)) i2=fgetc(File);
		ii=Integer2Byte(i1,i2);
		data.append(ii);
		Position+=2;
	};
	return data;
};

QString* ExcelBook::GetString(int num)
{
	if(num>=0 && num<(int)SharedStrings.count())
	{
		return SharedStrings[num];
	};
	return new QString("");
};

int ExcelBook::SeekBOF(void)
{
	int opcode,version,streamtype,length,ret=0;
	char *data;
	while(!feof(File))
	{
		opcode=Get2Bytes();
		if(opcode==XL_BOF)
		{
			length=Get2Bytes();
			data=Read(Position,length);
			version=Integer2Byte(data[0], data[1]);
			streamtype=Integer2Byte(data[2], data[3]);
			printf("SEEKBOF:opcode=XLBOF, %d ,version %d\r\n",Position,version);
			delete data; data=NULL;
			if (version==BIFF8) ret=8;
				else if(version==BIFF7) ret=7;
			printf("SEEKBOF:versionBIFF%d\r\n",ret);
			if(streamtype==WBKGLOBAL) return ret *2;
				else if(streamtype==WRKSHEET) return ret *1;
			return 1;
		};
	};
	return 0;
};

ExcelBREC* ExcelBook::GetBREC(void)
{
	ExcelBREC* rec;
	rec= new ExcelBREC;
	if(FileEOF()) return NULL;
	rec->data=NULL;
	rec->code=Get2Bytes();
	rec->length=Get2Bytes();
	rec->position=Position;
	SeekSkip(rec->length);
	return rec;
};

ExcelBREC* ExcelBook::PeekBREC(void)
{
	int oldpos;
	ExcelBREC* NextRec;
	oldpos=Position;
	NextRec=GetBREC();
	SeekPosition(oldpos);
	return NextRec;
};

char* ExcelBook::GetDataOfBREC(ExcelBREC* record)
{
	if(record->data==NULL)
	{
		ConvertCharToArray(record,Read(record->position,record->length),record->length);
	};
	return record->data;//new?
};

void ExcelBook::ConvertCharToArray(ExcelBREC* record, char* chars, int length)
{
	record->data=new char[length];
	for(int w1=0;w1<=length-1;w1++)
		record->data[w1]=chars[w1];
};


bool ExcelSheet::InitCells()
{
	int r;
	Cells.resize(rows * cols + cols+1);
	if(Cells.count()==0) return false;
	for(r=0;r < Cells.count();r++)
	{
		Cells[r]=NULL;
	};
	return true;
};

void ExcelSheet::Set(int row, int col, ExcelCell* cell)
{
	if(cell!=NULL&&(row*cols+col)<Cells.count())
	{
	Cells[row*cols+col]=cell;
	};
};

ExcelCell* ExcelSheet::Get(int row, int col)
{
	ExcelCell* cell;
	cell=Cells[row*cols+col];
	if(cell==NULL) return NULL;
	return cell;
};

int ExcelBook::SheetHandleRecord(ExcelSheet* sheet, ExcelBREC* record)
{
	char* data=NULL;
	switch (record->code)
	{
		case XL_DIMENSION:
		data = GetDataOfBREC(record);
			if (record->length == 10)
			{
				sheet->rows = Integer2Byte(data[2], data[3]);
				sheet->cols = Integer2Byte(data[6], data[7]);
			} else
			{
				sheet->rows = Integer4Byte(data[4], data[5], data[6], data[7]);
				sheet->cols = Integer2Byte(data[10], data[11]);
			}
			sheet->InitCells();
			break;

		case XL_LABELSST:
			HandleLabelSST(sheet, record);
			break;

		case XL_RK:
		case XL_RK2:
			HandleRK(sheet, record);
			break;

		case XL_MULRK:
			HandleMulrk(sheet, record);
			break;

		case XL_ROW:
			break;

		case XL_NUMBER:
			HandleNumber(sheet, record);
			break;

		case XL_BOOLERR:
			break;

		case XL_CONTINUE:
			break;

		case XL_FORMULA:
		case XL_FORMULA2:
			HandleFormula(sheet, record);
			break;

		case XL_LABEL:
			break;

		case XL_NAME:
			HandleName(sheet, record);
			break;

		case XL_BOF:
			break;
		case XL_EOF:
			return 0;
		default:
			break;
	};
	return 1;
};

int ExcelBook::ReadSheet(ExcelSheet* sheet)
{
	ExcelBREC* record;
	int oldpos;
	oldpos = Position;
	SeekPosition(sheet->position);
	record = GetBREC();
	while (record!=NULL)
	{
		if (!SheetHandleRecord(sheet, record)) break;
		record=GetBREC();
	};
	SeekPosition(oldpos);
	return 1;
};

ExcelSheet* ExcelBook::GetSheet(void)
{
	ExcelSheet* sh=NULL;
	int type;
	type=SeekBOF();
	Version=type;
	sh=new ExcelSheet;
	if(type)
	{
		sh->type=type;
		sh->position=Position;
		sh->name=QString("");
	};
	if(type==8||type==7)
	{
		ReadSheet(sh);
	};
	return sh;
};

void ExcelBook::ParseSheets(void)
{
	int BOFs;
	ExcelBREC* r;
	BOFs=1;
	r=GetBREC();
	while(BOFs)
	{
		r=GetBREC();
		switch(r->code)
		{
			case XL_SST:
				HandleSST(r);
				break;

			case XL_TXO:
				break;
			case XL_NAME:
				break;
			case XL_ROW:
				break;

			case XL_FORMAT:
				HandleFormat(r);
				break;

			case XL_XF:
				HandleXF(r);
				break;

			case XL_BOUNDSHEET:
				HandleBoundSheet(r);
				break;

			case XL_EXTSST:
				break;
			case XL_CONTINUE:
				break;

			case XL_EOF:
				BOFs--;
				break;

			default:
				break;
		};
	};
};

void ExcelBook::GetSheets(void)
{
	ExcelSheet* sheet;
	Sheets.resize(0);
	sheet=GetSheet();
	while (sheet->Cells.count()!= 0 )
	{
		Sheets.resize(Sheets.count()+1);
		Sheets[Sheets.count()-1]=sheet;
		sheet->name=*Names[Sheets.count()-1];
		sheet=GetSheet();
	};
};

bool ExcelBook::ParseBook(char *file)
{
	dateformat=QString("");
	DetectEndian();
	OpenFile(file);
	SeekBOF();
	ParseSheets();
	GetSheets();
	return true;
};

QString ExcelBook::GetASCII(char* inbytes, int pos, int chars)
{
	int i;
	QString outstr="";
	for (i = 0; i < chars; i++)
	{
		outstr.append(inbytes[i+pos]);
	};
	return outstr;
};

QString ExcelBook::GetUnicode(char * inbytes, int pos, int chars)
{
	QString outstr="";
	int i;
	int rc;
	for (i=0; i<chars*2; i++)
	{
		rc=Integer2Byte(inbytes[i+pos],inbytes[i+pos+1]);
		outstr.append(QChar(rc));
		i++;
	};
	return outstr;
};


void ExcelBook::HandleBoundSheet(ExcelBREC* rec)
{
	char* data;
	int type;
	int visibility;
	int length;
	int pos;
	QString name;
	pos = 8;
	data   = GetDataOfBREC(rec);
	type = data[4];
	visibility = data[5];
	length = data[6];
	if(data[7]==0)
	{
		//ascii
		name=GetASCII(data,pos,length);
	}else
	{
		name=GetUnicode(data,pos,length);
	};
	Names.resize(Names.count()+1);
	Names[Names.count()-1]=new QString(name);
};

void ExcelBook::HandleName(ExcelSheet* sheet, ExcelBREC* rec)
{
	char* data;
	QString name;
	int length;
	int pos;
	pos = 15;
	data   = GetDataOfBREC(rec);
	length = data[3];
	name = GetASCII(data,pos,length);


};

ExcelFormat* ExcelBook::GetFormatting(int xf)
{
	int i;
	ExcelFormat* rec;
	rec=new ExcelFormat();
	for (i = 0; formatter[i].code != 0; i++)
	{
		if (xf == formatter[i].code) break;
	};
	if (formatter[i].format ==NULL) return NULL;
	rec->code   = xf;
	rec->type   = formatter[i].type;
	rec->format = formatter[i].format;
	return rec;
};

void ExcelBook::HandleSetOfSST(ExcelBREC* rec/*, SSTList* cont*/, char* bytes)
{
	QString str=QString("");
	char* data;
	int chars, pos, options, i;
	int richstring, fareaststring, runlength=0;
	int richruns=0,fareastsize=0;
	int totalstrings;
	int uniquestrings;
	data = GetDataOfBREC(rec);
	totalstrings  = Integer4Byte(data[0], data[1], data[2], data[3]);
	uniquestrings = Integer4Byte(data[4], data[5], data[6], data[7]);
	pos = 8;
	for (i = 0; i < uniquestrings; i++)
	{
		richruns=0; fareastsize=0;
		chars = Integer2Byte(data[pos], data[pos+1]);
		pos += 2;
		options = data[pos];
		pos++;
		fareaststring = ((options & 0x04) != 0);
		richstring     = ((options & 0x08) != 0);
		if(richstring)
		{
			richruns= Integer2Byte(data[pos],data[pos+1]);
			pos+=2;
		};
		if(fareaststring)
		{
			fareastsize=Integer4Byte(data[pos], data[pos+1], data[pos+2], data[pos+3]);
			pos+=4;
		};

		if ((options & 0x01) == 0) //8 bit chars
		{
			/* ascii */
			str = GetASCII(bytes,pos,chars);
			pos=pos+chars;
			if(str[0]=='=') str[0]=' ';
		}else //16 bit chars
		{
			/* unicode */
			str = GetUnicode(bytes,pos,chars);
			pos=pos+chars*2;
		};
		// HERE TO PUT richformat handling
		if (richstring)
		{
			pos += 4 * richruns;
		};
		if (fareaststring)
		{
			pos += fareastsize;
		};
		//printf("String=%s, length=%d first=0x%x\r\n",str.ascii(),str.length(),str[0].unicode());
		SharedStrings.resize(SharedStrings.count()+1);
		SharedStrings[SharedStrings.count()-1]=new QString(str);
	}
};


char* ExcelBook::MergeBytesFromSSTs(ExcelBREC* rec,SSTList* cont)
{
	int i, pos;
	int length;

	char* data;
	char* bytes;
	length = rec->length;
	for (i = 0; i < (int) cont->rec.count(); i++)
	{
		length += cont->rec[i]->length;
	}
	bytes = GetDataOfBREC(rec);
	pos = rec->length;
	for (i = 0; i < (int) cont->rec.count(); i++)
	{
		data = GetDataOfBREC(cont->rec[i]);
		*bytes += pos;
		bytes = data;
		pos += cont->rec[i]->length;
	}
	return bytes;
};


void ExcelBook::HandleSST(ExcelBREC* rec)
{
	char* bytes;
	SSTList* cont;
	cont= new SSTList;
	ExcelBREC* nr;
	nr = PeekBREC();
	while (nr->code == XL_CONTINUE)
	{
		cont->rec.resize(cont->rec.count()+1);
		cont->rec[cont->rec.count()-1]=GetBREC();
		nr = PeekBREC();
	}
	bytes = MergeBytesFromSSTs(rec,cont);
	HandleSetOfSST(rec, bytes);
	for(int w1=0;w1<(int)cont->rec.count();w1++)
	{
		if(cont->rec[w1]!=NULL) {delete cont->rec[w1];cont->rec[w1]=NULL;};
	};
	cont->rec.resize(0);
};

void ExcelBook::HandleLabelSST(ExcelSheet* sheet, ExcelBREC* rec)
{
	int index, row, col;
	char* data;
	data = GetDataOfBREC(rec);
	index = Integer4Byte(data[6], data[7], data[8], data[9]);
	row = Integer2Byte(data[0], data[1]);
	col = Integer2Byte(data[2], data[3]);
	sheet->Set(row,col, CellLabel(row, col, *GetString(index)));
};

ExcelCell* ExcelBook::CellLabel(int row, int col, QString str)
{
	ExcelCell* c;
	c= new ExcelCell;
	c->row = row;
	c->col = col;
	c->type = CELL_LABEL;
	c->valuec = str;
	return c;
};

ExcelCell* ExcelBook::CellNumber(int row, int col, int index, double d)
{
	ExcelCell* c;
	c=new ExcelCell;
	c->row = row;
	c->col = col;
	c->xfindex = index;
	c->type = CELL_NUMBER;
	c->valued = d;
	return c;
};

QString* ExcelBook::CellDataString(ExcelSheet* sh, int row, int col)
{
	time_t date;
	struct tm *tmptr;
	ExcelCell* c;
	char str[128];
	QString format;
	int precision;
	int utcOffsetDays = 25569;
	int sInADay = 24 * 60 * 60;
	c = sh->Get(row,col);
	if (c == NULL) return new QString("");
	switch (c->type)
	{
		case CELL_LABEL:
			return new QString(c->valuec);
		case CELL_NUMBER:
			if (XFRecords[c->xfindex]->type == DATEFORMAT)
			{

				format = XFRecords[c->xfindex]->format;
				date = (time_t) ((c->valued - utcOffsetDays) * sInADay);
				tmptr = gmtime(&date);
				if (dateformat)
				{
					strftime(str,1024,dateformat.ascii(),tmptr);
				} else
				{
					strftime(str,1024,format.ascii(),tmptr);
				};
			} else
			if (XFRecords[c->xfindex]->type == NUMBERFORMAT)
			{
				format = XFRecords[c->xfindex]->format;
				//sprintf(str,format.ascii(),c->valued);
				// the real format is ignored...
				// because there is more work to be done in the field
				precision = CellGetPrecision(c->valued);
				sprintf(str,"%.*f",precision,c->valued);
			}else
			{
				precision = CellGetPrecision(c->valued);
				sprintf(str,"%.*f",precision,c->valued);
			};
			break;
		case CELL_DATE:
			break;
		case CELL_BOOLEAN:
			break;
		case CELL_ERROR:
			break;
	}
	return new QString(str);
};

int ExcelBook::CellGetPrecision(double d)
{
	double t;
	int i,x;
	int count;
	if (d < 0) d *= -1;
	i = (int)d;
	t = d - (double)i;
	if (t <= 0)
	{
		return 0;
	};
	count = 0;
	for (x = 6; x > 1; x--)
	{
		i = (int)d;
		t = d - (double)i;
		t *= pow(10,x - 2);
		i = (int)t;
		t = t - (double)i;
		t *= 10;
		i = (int)t;
		if (i > 0) break;
		count++;
	};
	return (5 - count);
};


void ExcelBook::CellSetDateFormat(char *d)
{
	dateformat = QString(d);
};

void ExcelBook::HandleMulrk(ExcelSheet* sheet, ExcelBREC* record)
{
	struct mulrk mulrk;
	char* data;
	ExcelCell* cell;
	int len;
	int i;
	len = record->length;
	data = GetDataOfBREC(record);
	mulrk.row   = Integer2Byte(data[0],data[1]);
	mulrk.first = Integer2Byte(data[2],data[3]);
	mulrk.last  = Integer2Byte(data[len - 2],data[len - 1]);
	mulrk.numrks = mulrk.last - mulrk.first + 1;
	MulrkRead(&mulrk, data);
	for (i = 0; i < mulrk.numrks; i++)
	{
		cell = CellNumber(mulrk.row, mulrk.first + i, mulrk.xfindices[i], mulrk.rkdbls[i]);
		sheet->Set(mulrk.row,mulrk.first+ i, cell);
		//printf("handleMULRK:row=%d,col=%d,val=%f\r\n",mulrk.row,mulrk.first+i,mulrk.rkdbls[i]);
	}
	//delete(mulrk.xfindices);
	//delete(mulrk.rkdbls);
};

void ExcelBook::MulrkRead(struct mulrk *mulrk, char* data)
{
	double d;
	int i;
	int pos;
	pos = 4;
	mulrk->xfindices.resize(mulrk->numrks);
	mulrk->rkdbls.resize(mulrk->numrks);
	for (i = 0; i < mulrk->numrks; i++)
	{
		mulrk->xfindices[i] = Integer2Byte(data[pos], data[pos+1]);
		d=Double4Byte(data[pos+2], data[pos+3], data[pos+4], data[pos+5]);
		//printf("double:%f\r\n",d);
		mulrk->rkdbls[i] = d;
		pos += 6;
	}
};


void ExcelBook::HandleNumber(ExcelSheet* sheet, ExcelBREC* record)
{
	int xfindex, row, col;
	char* data;
	double d;
	data = GetDataOfBREC(record);
	row = Integer2Byte(data[0], data[1]);
	col = Integer2Byte(data[2], data[3]);
	xfindex = Integer2Byte(data[4], data[5]);
	#if defined(__arm__) && !defined(__vfp__)
		d=Double8Byte(data[10], data[11], data[12], data[13],data[6], data[7], data[8], data[9]);
	#else
		d=Double8Byte(data[6], data[7], data[8], data[9],data[10], data[11], data[12], data[13]);
	#endif
	//even if ARM is little endian... doubles are been placed as bigendian words.
	//thanks pb_ for that. :)
	sheet->Set(row,col, CellNumber(row,col,xfindex,d));
	//printf("handleNumber:row=%d,col=%d,val=%f\r\n",row,col,d);
};

ExcelFormat::ExcelFormat()
{
code=0;type=0;format="";
};

ExcelFormat::ExcelFormat(int c,int t, QString s)
{
	code=c;type=t;format=s;
};


void ExcelBook::HandleFormat(ExcelBREC* rec)
{
	ExcelFormat* xfrec;
	char* data;
	int format;
	data   = GetDataOfBREC(rec);
	format = Integer2Byte(data[2],data[3]);
	xfrec  = GetFormatting(format);
	/*int idx;
	idx=XFRecords.count()-1;
	XFRecords[idx]->code=xfrec->code;
	XFRecords[idx]->type=xfrec->type;
	XFRecords[idx]->format="manos";
	//XFRecords[XFRecords.count()-1]=xfrec;
	printf("6\r\n");*/
};

void ExcelBook::HandleXF(ExcelBREC* rec)
{
	ExcelFormat* xfrec;
	char* data;
	int format;
	data   = GetDataOfBREC(rec);
	format = Integer2Byte(data[2],data[3]);
	xfrec  = GetFormatting(format);
	XFRecords.resize(XFRecords.count()+1);
	XFRecords[XFRecords.count()-1]=xfrec;
};



void ExcelBook::HandleRK(ExcelSheet* sheet, ExcelBREC* record)
{
	int xfindex, row, col;
	char* data;
	double d;
	data = GetDataOfBREC(record);
	row = Integer2Byte(data[0], data[1]);
	col = Integer2Byte(data[2], data[3]);
	xfindex = Integer2Byte(data[4], data[5]);
	d=Double4Byte(data[6], data[7], data[8], data[9]);
	sheet->Set(row,col,CellNumber(row,col,xfindex,d));
	//printf("handleRK:row=%d,col=%d,val=%f\r\n",row,col,d);
};


void ExcelBook::HandleFormula(ExcelSheet* sheet, ExcelBREC* record)
{
	int xfindex, row, col;
	char* data;
	double d;
	data = GetDataOfBREC(record);
	row = Integer2Byte(data[0], data[1]);
	col = Integer2Byte(data[2], data[3]);
	if (data[6] == 0 && data[12] == -1 && data[13] == -1)
	{
		// string
	} else
	if (data[6] == 1  && data[12] == -1 && data[13] == -1)
	{
		// boolean
	} else
	if ( data[6] == 2 && data[12] == -1 && data[13] == -1)
	{
		// error
	}
	else
	{
		// number
		xfindex = Integer2Byte(data[4], data[5]);
		d=Double8Byte(data[6], data[7], data[8], data[9],data[10], data[11], data[12], data[13]);
		QString s1;
		int sz;
		sz=Integer2Byte(data[20],data[21]);// size of the formula
		char* formuladata;
		formuladata=new char[sz];
		for(int w1=0;w1<sz;w1++)
		{
			formuladata[w1]=data[22+w1];
		};
		//22 is the first 0 idx of formula data
		s1="="+GetFormula(row,col,sheet,formuladata,sz);
		//printf("GetFormula:Formula=%s\r\n",s1.ascii());
		sheet->Set(row,col,CellLabel(row,col,s1));
	}
};


QString ExcelBook::GetFormula(int row, int col, ExcelSheet* sheet, char* data, int sz)
{
	int length=sz;
	printf("{FormulaParser}\r\n");
	printf("row=%d, col=%d, length=%d\r\n",row,col,length);
	int idx=0;
	int w1,w2,w3,w4;
	double d1;
	int token;
	QString s1;
	QList <QString> operands;
	operands.setAutoDelete(TRUE);
	QString formula;
	operands.clear();
	while( idx<length )
	{
		token= data[idx]; idx++;
		switch(token)
		{
			case 0x1E: //prtInt
				w1=Integer2Byte(data[idx],data[idx+1]);
				idx=idx+2;
				operands.prepend(new QString(QString::number(w1)));
				printf(" token:ptgInt,num=%d\r\n",w1);
			break;
			case 0x1F: //ptgNumber
				#if defined(__arm__) && !defined(__vfp__)
					d1=Double8Byte(data[idx+4],data[idx+5],data[idx+6],data[idx+7]
						,data[idx],data[idx+1],data[idx+2],data[idx+3]);
				#else
					d1=Double8Byte(data[idx],data[idx+1],data[idx+2],data[idx+3]
						,data[idx+4],data[idx+5],data[idx+6],data[idx+7]);
				#endif
				idx=idx+8;
				operands.prepend(new QString(QString::number(d1)));
				printf(" token:ptgNumber,num=%f\r\n",d1);
			break;
			case 0x17: //ptgStr
				if(Version==8)
				{
					//unicode string
					//w1=Integer2Byte(data[idx],data[idx+1]);idx+=2;
					w1=data[idx];idx++;
					printf("len=%d\r\n",w1);
					int richruns=0; int fareastsize=0;
					int richstring,fareaststring;
					int options = data[idx];idx++;
					fareaststring = ((options & 0x04) != 0);
					richstring     = ((options & 0x08) != 0);
					if(richstring)
					{
						//containts rich string formatting.
						printf("STRING:richstring\r\n");
						richruns= Integer2Byte(data[idx],data[idx+1]);
						printf("richruns:%d\r\n",richruns);
						idx+=2;
					};
					if(fareaststring)
					{
						//contains far east formatting
						printf("STRING:fareast!\r\n");
						fareastsize=Integer4Byte(data[idx], data[idx+1],
							data[idx+2], data[idx+3]);
						printf("fareastsize=%d",fareastsize);
						idx+=4;
					};
					if ((options & 0x01) == 0) //8 bit chars
					{
						/* ascii */
						s1 = GetASCII(data,idx,w1);
						idx=idx+w1;
						printf("STRING:ASCII=%s\r\n",s1.ascii());
					}else //16 bit chars
					{
						/* unicode */
						s1 = GetUnicode(data,idx,w1);
						idx=idx+w1*2;
						printf("STRING:unicode=%s\r\n",s1.ascii());
					};
					// HERE TO PUT richformat handling
					if (richstring)
					{
						idx += 4 * richruns;
					};
					if (fareaststring)
					{
						idx += fareastsize;
					};
					s1=QString("""")+s1+QString("""");
					operands.prepend(new QString(s1));
				}else
				{
					w1=data[idx];idx++;
					s1=GetASCII(data,idx,w1);
					s1=QString("""")+s1+QString("""");
					idx=idx+w1;
     					operands.prepend(new QString(s1));
				};
				printf(" token:ptgStr,num=%d\r\n",w1);
			break;
			case 0x25:
			case 0x45:
			case 0x65: // ptgArea
				if(Version==8)
				{
					w1=Integer2Byte(data[idx],data[idx+1]);idx=idx+2;//row1
					w2=Integer2Byte(data[idx],data[idx+1]);idx=idx+2;//row2
					w3=Integer2Byte(data[idx],data[idx+1]) & 0x00FF;idx=idx+2;//col1
					w4=Integer2Byte(data[idx],data[idx+1]) & 0x00FF;idx=idx+2;//col2
				}else
				{
					w1=Integer2Byte(data[idx],data[idx+1]) & 0x3FFF;idx=idx+2;//row1
					w2=Integer2Byte(data[idx],data[idx+1]) & 0x3FFF;idx=idx+2;//row2
					w3=Integer2Byte(data[idx],data[idx+1]);idx=idx+2;//col1
					w4=Integer2Byte(data[idx],data[idx+1]);idx=idx+2;//col2
				};
				//ignores relative or absolute refs
				s1=FindCellName(w1,w3)+":"+FindCellName(w2,w4);
				printf(" token:ptgArea,ref=%s\r\n",s1.ascii());
				operands.prepend(new QString(s1));
			break;
			case 0x24:
			case 0x44:
			case 0x64://ptgRef
				if(Version==8)
				{
					w1=Integer2Byte(data[idx],data[idx+1]);idx=idx+2;//row
					w2=Integer2Byte(data[idx],data[idx+1]) & 0x00FF;idx=idx+2;//col
				}else
				{
					w1=Integer2Byte(data[idx],data[idx+1]) & 0x3FFF;idx=idx+2;//row
					w2=Integer2Byte(data[idx],data[idx+1]);idx=idx+2;//col
				};
				s1=FindCellName(w1,w2);
				printf("token:ptgRef,ref=%s\r\n",s1.ascii());
				operands.prepend(new QString(s1));
			break;
			case 0x1D: // ptgBool
				w1=data[idx];idx++;
				printf("token:ptgBool,val=%d\r\n",w1);
				operands.prepend(new QString(QString::number(w1)));
			break;
			case 0x16://ptg MissArg
				printf("token:ptgMissArg, val=' '\r\n");
				operands.prepend(new QString("0"));
			break;
			case 0x12://ptgUplus==
				printf("token:ptgUplus\r\n");
				s1=QString("+")+operands.first()->ascii();
				operands.removeFirst();
				operands.prepend(new QString(s1));
			break;
			case 0x13://ptgUminus
				printf("token:ptgUminus\r\n");
				s1=QString("-")+operands.first()->ascii();
				operands.removeFirst();
				operands.prepend(new QString(s1));
			break;
			case 0x03://ptgAdd
				printf("token:ptgAdd\r\n");
				operands.first();
				s1=operands.next()->ascii()
						+QString("+")+operands.first()->ascii();
				operands.removeFirst();operands.removeFirst();
				operands.prepend(new QString(s1));
			break;
			case 0x04://ptgSub
				printf("token:ptgSub\r\n");
				operands.first();
				s1=operands.next()->ascii()
						+QString("-")+operands.first()->ascii();
				operands.removeFirst();operands.removeFirst();
				operands.prepend(new QString(s1));
			break;
			case 0x05://ptgMul
				printf("token:ptgMul\r\n");
				operands.first();
				s1=operands.next()->ascii()
						+QString("*")+operands.first()->ascii();
				operands.removeFirst();operands.removeFirst();
				operands.prepend(new QString(s1));
			break;
			case 0x06://ptgDiv
				printf("token:ptgDiv\r\n");
				operands.first();
				s1=operands.next()->ascii()
						+QString("/")+operands.first()->ascii();
				operands.removeFirst();operands.removeFirst();
				operands.prepend(new QString(s1));
			break;
			case 0x07://ptgPOWER
				printf("token:ptgPow\r\n");
				operands.first();
				s1=QString("POWER(")+operands.next()->ascii()
						+QString(",")+operands.first()->ascii()+QString(")");
				operands.removeFirst();operands.removeFirst();
				operands.prepend(new QString(s1));
			break;
			case 0x08://ptgConcat
				printf("token:ptgConcat\r\n");
				operands.first();
				s1=QString("CONCATENATE(")+operands.next()->ascii()
						+QString(",")+operands.first()->ascii()+QString(")");
				operands.removeFirst();operands.removeFirst();
				operands.prepend(new QString(s1));
			break;
			case 0x15://ptgParenthesis
				printf("token:ptgParenthesis\r\n");
				s1=QString("(")+operands.first()->ascii()+QString(")");
				operands.removeFirst();
				operands.prepend(new QString(s1));
			break;
			case 0x14://ptgPercent
				printf("token:ptgPercent\r\n");
				s1=operands.first()->ascii()+QString("*0.01");
				operands.removeFirst();
				operands.prepend(new QString(s1));
			break;
			case 0x9://ptgLessThan
				printf("token:ptgLESS\r\n");
				operands.first();
				s1=operands.next()->ascii()
						+QString("<")+operands.first()->ascii();
				operands.removeFirst();operands.removeFirst();
				operands.prepend(new QString(s1));
			break;
			case 0xa://ptgLessEqual
				printf("token:ptgLESS_EQUAL\r\n");
				operands.first();
				s1=operands.next()->ascii()
						+QString("<=")+operands.first()->ascii();
				operands.removeFirst();operands.removeFirst();
				operands.prepend(new QString(s1));
			break;
			case 0xb://ptgEQUAL
				printf("token:ptgEQUAL\r\n");
				operands.first();
				s1=operands.next()->ascii()
						+QString("==")+operands.first()->ascii();
				operands.removeFirst();operands.removeFirst();
				operands.prepend(new QString(s1));
			break;
			case 0xc://ptgGREATER_EQUAL
				printf("token:ptgGREAT_EQUAL\r\n");
				operands.first();
				s1=operands.next()->ascii()
						+QString(">=")+operands.first()->ascii();
				operands.removeFirst();operands.removeFirst();
				operands.prepend(new QString(s1));
			break;
			case 0xd://ptgGREAT_THAN
				printf("token:ptgGREAT_THAN\r\n");
				operands.first();
				s1=operands.next()->ascii()
						+QString(">")+operands.first()->ascii();
				operands.removeFirst();operands.removeFirst();
				operands.prepend(new QString(s1));
			break;
			case 0xe://ptgNOT_EQUAL
				printf("token:ptgNOTequal\r\n");
				operands.first();
				s1=operands.next()->ascii()
						+QString("!=")+operands.first()->ascii();
				operands.removeFirst();operands.removeFirst();
				operands.prepend(new QString(s1));
			break;
			case 0x19://attribute can be Sum,If,Choose
				w3=Integer2Byte(data[idx],data[idx+1]);idx=idx+2;
				idx++;
				printf("token:ATTRIBUTE:0x%x\r\n",w3);
				for(w4=idx;w4<length;w4++)
					printf("0x%x, ",data[w4]);
				if(w3&0x01)//choose
				{
					printf("token:CHOOSE\r\n");
				}
				else if(w3&0x02)//if
				{
					printf("token:IF\r\n");
				}
				else if(w3&0x10)//sum
				{
					printf("token:SUM\r\n");
				};

			break;


			case 0x21:
			case 0x22:
			case 0x42:
			case 0x62:
			case 0x41:
			case 0x61://ptgFunction
				printf("token:ptgFunction\r\n");
				if(token==0x22||token==0x42||token==0x62)
				{
					w2=(int)data[idx];idx++;
					w1=Integer2Byte(data[idx],data[idx+1]);idx=idx+2;
				}else
				{
					w1=Integer2Byte(data[idx],data[idx+1]);idx=idx+2;
				};
				switch(w1)
				{
					case 0xf://SIN
						s1=QString("SIN(")+operands.first()->ascii()+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0x10://COS
						s1=QString("COS(")+operands.first()->ascii()+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0x11://tan
						s1=QString("TAN(")+operands.first()->ascii()+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0x62://asin
						s1=QString("ASIN(")+operands.first()->ascii()+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0x63://ACOS
						s1=QString("ACOS(")+operands.first()->ascii()+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0x12://ATAN
						s1=QString("ATAN(")+operands.first()->ascii()+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0xe5://SINH
						s1=QString("SINH(")+operands.first()->ascii()+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0xe6://COSH
						s1=QString("COSH(")+operands.first()->ascii()+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0xe7://TANH
						s1=QString("TANH(")+operands.first()->ascii()+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0xe8://ASINH
						s1=QString("ASINH(")+operands.first()->ascii()+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0xe9://ACOSH
						s1=QString("ACOSH(")+operands.first()->ascii()+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0xea://ATANH
						s1=QString("ATANH(")+operands.first()->ascii()+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0x13://pi
						s1="PI()";
						operands.prepend(new QString(s1));
					break;
					case 0x14://sqrt
						s1=QString("SQRT(")+operands.first()->ascii()+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0x15://exp
						s1=QString("EXP(")+operands.first()->ascii()+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0x16://LN
						s1=QString("LN(")+operands.first()->ascii()+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0x17://LOG10
						s1=QString("LOG10(")+operands.first()->ascii()+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0x18://ABS
						s1=QString("ABS(")+operands.first()->ascii()+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0x19://int
						s1=QString("INT(")+operands.first()->ascii()+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0x1a://sign
						s1=QString("SIGN(")+operands.first()->ascii()+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0x1b://round
						operands.first();
						s1=QString("ROUND(")+operands.next()->ascii()
							+QString(",")+operands.first()->ascii()
								+QString(")");
						operands.removeFirst();operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0x1d://index
						operands.first();
						s1=QString("INDEX(")+operands.next()->ascii()
							+QString(",")
							+operands.first()->ascii()+QString(")");
						operands.removeFirst();
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0x1: // if ATTRIBUTE
						operands.first();operands.next();
						s1=QString("IF(")+operands.next()->ascii()+QString(",");
						operands.first();
						s1=s1+operands.next()->ascii()+QString(",");
						s1=s1+operands.first()->ascii()+QString(")");
						operands.removeFirst();
						operands.removeFirst();
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0x81://isblank
						s1=QString("ISBLANK(")+operands.first()->ascii()
							+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0x80://isnumber
						s1=QString("ISNUMBER(")+operands.first()->ascii()
							+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0x120://ceiling
						operands.first();
						s1=QString("CEILING(")+operands.next()->ascii()
							+QString(",")+operands.first()->ascii()
								+QString(")");
						operands.removeFirst();operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0x11d://floor
						operands.first();
						s1=QString("FLOOR(")+operands.next()->ascii()
							+QString(",")+operands.first()->ascii()
								+QString(")");
						operands.removeFirst();operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0x157://degrees
						s1=QString("DEGREES(")+operands.first()->ascii()
							+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0x156://radians
						s1=QString("RADIANS(")+operands.first()->ascii()
							+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0xb8://fact
						s1=QString("FACT(")+operands.first()->ascii()
							+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0x27://MOD
						operands.first();
						s1=QString("MOD(")+operands.next()->ascii()
							+QString(",")+operands.first()->ascii()
								+QString(")");
						operands.removeFirst();operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0x151://power
						operands.first();
						s1=QString("POWER(")+operands.next()->ascii()
							+QString(",")+operands.first()->ascii()
								+QString(")");
						operands.removeFirst();operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0x3f://rand()
						s1="RAND()";
						operands.prepend(new QString(s1));
					break;
					case 0x4://sum
						for(w4=1;w4<w2;w4++) operands.removeFirst();
						s1=QString("SUM(")+operands.first()->ascii()
									+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0x6://min
						for(w4=1;w4<w2;w4++) operands.removeFirst();
						s1=QString("MIN(")+operands.first()->ascii()
									+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0x7://max
						for(w4=1;w4<w2;w4++) operands.removeFirst();
						s1=QString("MAX(")+operands.first()->ascii()
									+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0x5://average
						for(w4=1;w4<w2;w4++) operands.removeFirst();
						s1=QString("AVERAGE(")+operands.first()->ascii()
									+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0x2e://var
						for(w4=1;w4<w2;w4++) operands.removeFirst();
						s1=QString("VAR(")+operands.first()->ascii()
									+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0xc2://varp
						for(w4=1;w4<w2;w4++) operands.removeFirst();
						s1=QString("VARP(")+operands.first()->ascii()
									+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0xc://stdev
						for(w4=1;w4<w2;w4++) operands.removeFirst();
						s1=QString("STDEV(")+operands.first()->ascii()
									+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0xc1://stdevp
						for(w4=1;w4<w2;w4++) operands.removeFirst();
						s1=QString("STDEVP(")+operands.first()->ascii()
									+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0x143://skew
						for(w4=1;w4<w2;w4++) operands.removeFirst();
						s1=QString("SKEW(")+operands.first()->ascii()
									+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0x142://kurt
						for(w4=1;w4<w2;w4++) operands.removeFirst();
						s1=QString("KURT(")+operands.first()->ascii()
									+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;
					case 0x0://count
						for(w4=1;w4<w2;w4++) operands.removeFirst();
						s1=QString("COUNT(")+operands.first()->ascii()
									+QString(")");
						operands.removeFirst();
						operands.prepend(new QString(s1));
					break;

					default:
						printf("token:FUNCTION_UNKNOWN=0x%x\r\n",w1);
						return QString("FUNC_UNKNOWN");
					break;

				};

			break;

			default:
				printf("tokenUNKNOWN=0x%x\r\n",token);
				return QString("TOKEN_UKNOWN");
				//it is dangerous to go to idx++ and not return
				// because the result is unexpected.
				// but there is a possibility the the parser will give the correct
				// answer, because there are some tokens in excel formulas that can be					//ignored.
				idx++;
			break;
		};

	};



	printf("{////FormulaParser}\r\n");
	printf("GetFormula:::::::r=%d,c=%d,,,%s\r\n",row,col,s1.ascii());
	printf("\r\n");
	s1=operands.first()->ascii();
	operands.clear();
return QString(s1);
};

QString ExcelBook::FindCellName(int row, int col)
{
  row++;col++;
  QString s1="";
  int i1=col % 26;
  int i2=col / 26;
  if (i2!=0) s1=(char)(i2+65); //65 =A
  s1=s1+(char)(i1+65-1);
  return (s1+QString::number(row));
};








