#include "pdb.h"
#ifdef _WINDOWS
#include <winsock2.h>
#endif

#ifdef USEQPE
void Cpdb::suspend()
{
  CExpander::suspend(fin);
}
void Cpdb::unsuspend()
{
  CExpander::unsuspend(fin);
}
#endif

Cpdb::~Cpdb()
{
  if (fin != NULL)
    {
#ifdef USEQPE
      unsuspend();
#endif
      fclose(fin);
    }
}

size_t Cpdb::recordpos(int n)
{
    UInt16 mxn = ntohs(head.recordList.numRecords);
    if (n >= mxn)
    {
	return file_length;
    }
    else
    {
	size_t dataoffset = sizeof(DatabaseHdrType) - sizeof(UInt16);
	dataoffset += /*dataoffset%4 + */ sizeof(RecordListType) * n;
	unsuspend();
	fseek(fin, dataoffset, SEEK_SET);
	RecordListType hdr;
	fread(&hdr, 1, sizeof(hdr), fin);
	return ntohl(hdr.nextRecordListID);
    }

}

size_t Cpdb::recordlength(int n)
{
    return recordpos(n+1)-recordpos(n);
}

void Cpdb::gotorecordnumber(int n)
{
unsuspend();
    fseek(fin, recordpos(n), SEEK_SET);
}

bool Cpdb::openpdbfile(const char *src)
{
  //qDebug("cpdb openfile:%s", src);

    //  printf("In openfile\n");
    //  printf("closing fin:%x\n",fin);
    if (fin != NULL) fclose(fin);
    //  printf("opening fin\n");
    fin = fopen(src,"rb");

    if (fin==0)
    {
	return false;
    }

    // just holds the first few chars of the file
    //	char buf[0x100];
      struct stat _stat;
      stat(src,&_stat);
      file_length = _stat.st_size;
      //    fseek(fin,0,SEEK_END);
      //    file_length = ftell(fin);

    fseek(fin,0,SEEK_SET);

    fread(&head, 1, sizeof(head), fin);

    //qDebug("Database name:%s", head.name);
    //qDebug("Total number of records:%u", ntohs(head.recordList.numRecords));

    return true;
}
