#include "pdb.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

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
    fseek(fin, recordpos(n), SEEK_SET);
}

bool Cpdb::openfile(const char *src)
{

    //  printf("In openfile\n");
    int ret = 0;
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
    struct stat buf;
    stat(src, &buf);
    file_length = buf.st_size;
//    fseek(fin,0,SEEK_END);
//    file_length = ftell(fin);

//    fseek(fin,0,SEEK_SET);

    fread(&head, 1, sizeof(head), fin);

    return true;
}
