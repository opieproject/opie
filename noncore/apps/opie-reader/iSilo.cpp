#include "iSilo.h"
#ifdef _WINDOWS
#include <winsock.h>
#endif
u_int8_t *rodata =  (u_int8_t *)
  "\x10\x11\x12\x00\x08\x07\x09\x06\x0a\x05\x0b\x04\x0c\x03\x0d\x02\x0e\x01\x0f";

u_int16_t *rsize_min = (u_int16_t *)
  "\x03\x00\x04\x00\x05\x00\x06\x00\x07\x00\x08\x00\x09\x00\x0a\x00\x0b\x00\x0d"
"\x00\x0f\x00\x11\x00\x13\x00\x17\x00\x1b\x00\x1f\x00\x23\x00\x2b\x00\x33\x00"
"\x3b\x00\x43\x00\x53\x00\x63\x00\x73\x00\x83\x00\xa3\x00\xc3\x00\xe3\x00\x02"
"\x01";

u_int8_t *rsize_delta = (u_int8_t *)
  "\x00\x00\x00\x00\x00\x00\x00\x00\x01\x01\x01\x01\x02\x02\x02\x02\x03\x03\x03"
"\x03\x04\x04\x04\x04\x05\x05\x05\x05\x00";

u_int16_t *rpos_min = (u_int16_t *)
  "\x01\x00\x02\x00\x03\x00\x04\x00\x05\x00\x07\x00\x09\x00\x0d\x00\x11\x00\x19"
"\x00\x21\x00\x31\x00\x41\x00\x61\x00\x81\x00\xc1\x00\x01\x01\x81\x01\x01\x02"
"\x01\x03\x01\x04\x01\x06\x01\x08\x01\x0c\x01\x10\x01\x18\x01\x20\x01\x30\x01"
"\x40\x01\x60";

u_int8_t *rpos_delta = (u_int8_t *) 
  "\x00\x00\x00\x00\x01\x01\x02\x02\x03\x03\x04\x04\x05\x05\x06\x06\x07\x07"
"\x08\x08\x09\x09\x0a\x0a\x0b\x0b\x0c\x0c\x0d\x0d";

void iSilo::init_tables(void)
{
  int i;
  u_int16_t j;
  return;  
  for (i = 0; i < 3; i++)
    rodata[i] = 0x10 + i;
  rodata[3] = 0;
  for (i = 4; i < 19; i = i + 2)
    rodata[i] = 8 + (i-4)/2;
  for (i = 5; i < 19; i = i + 2)
    rodata[i] = 7 - (i-5)/2;
  
  memset(rsize_delta, 0, 29);
  for (i = 4; i < 29; i++) {
    rsize_delta[i] = (i - 4) >> 2;
  }
  memset(rpos_delta, 0, 30);
  for (i = 2; i < 30; i++) {
    rpos_delta[i] = (i - 2) >> 1;
  }      
  j = 3;
  for (i = 0; i < 29; i++) {
    rsize_min[i] = j;
    j += 1 << rsize_delta[i];
  }
  j = 1;
  for (i = 0; i < 30; i++) {
    rpos_min[i] = j;
    j += 1 << rpos_delta[i];
  }
}

/* read num bits from the file descriptor */

int iSilo::code2tree(struct s_huffman *h) {
  struct s_tree *t, *tmp;
  u_int32_t i;
  int j, b;
  
  t = new s_tree();
  h->tree = t;
  if (t == NULL) return -1;
  memset(t, 0, sizeof(struct s_tree));
  t->value = (u_int32_t)-1;
  for (i = 0; i < h->num; i++) {
    tmp = t;
    for (j = 0; j < h->size[i]; j++) {
      b = (h->code[i] >> j) & 1;
      if (tmp->branch[b] == NULL) {
	tmp->branch[b] =new s_tree();
	if (tmp->branch[b] == NULL) return(-1);
	memset(tmp->branch[b], 0, sizeof(struct s_tree));
	tmp->value = (u_int32_t)-1;
      }
      tmp = tmp->branch[b]; 
    }
    if (h->size[i] > 0) tmp->value = i;
  }
  return (0);
}

u_int32_t iSilo::swap_bits(u_int32_t n, int num) {
  n = ((n >>  1) & 0x55555555) | ((n <<  1) & 0xaaaaaaaa);
  n = ((n >>  2) & 0x33333333) | ((n <<  2) & 0xcccccccc);
  n = ((n >>  4) & 0x0f0f0f0f) | ((n <<  4) & 0xf0f0f0f0);
  n = ((n >>  8) & 0x00ff00ff) | ((n <<  8) & 0xff00ff00);
  n = ((n >> 16) & 0x0000ffff) | ((n << 16) & 0xffff0000);
  n >>= (32 - num);
  return(n);                  
}


u_int32_t *iSilo::huffman_get(struct s_huffman *h)
{
  int b;
  struct s_tree *t = h->tree;
  
  while (t->value == -1) {
    b = get_bits(1);
    if (t->branch[b] == NULL) {
      return(NULL);
    }
    t = t->branch[b];
  } 
  return (&t->value);
}

int iSilo::size2code(struct s_huffman *h) {
  u_int8_t i, sk, max = 0;
  u_int16_t sc, c;
  u_int32_t j, k, l;
  int skip;
  
  for (l = 0; l < h->num; l++) {
    if (h->size[l] > max) max = h->size[l];
  }
  
  for (i = 1; i <= max; i++) {
    sc = 0;
    c = 0; 
    for (j = 0; j < h->num; j++) {
      if (h->size[j] == i) {
	do {
	  skip = 0;
	  for (k = 0; k < h->num; k++) {
	    sk = h->size[k];
            
	    if ((sk < i) && (sk != 0) && !((h->code[k] ^ c) & ((1 << sk)-1))) {
	      if ((c + 1) == (1 << i)) {
		return -1;
	      }
	      sc += 1 << (i-sk);
	      c = swap_bits(sc, i);
	      skip = 1;
	      break;
	    }
	  }
	} while (skip);
	h->code[j] = c;
	sc++;
	c = swap_bits(sc, i);
      }
    }  
  }  
  return(0);
}


struct s_huffman *iSilo::huffman_create(u_int32_t num) {
  struct s_huffman *h;
  h = new s_huffman();
  h->tree = new s_tree();
  memset(h->tree, 0, sizeof(struct s_tree));
  h->tree->value = (u_int32_t)-1;
  h->num = num;
  h->code = new u_int16_t[num];
  h->size = new u_int8_t[num];
  memset(h->size, 0, num);
  memset(h->code, 0, num*2);
  return (h);
}

void iSilo::kill_tree(struct s_tree *tree) {
  if (tree == NULL) {
    return;
  }
  kill_tree(tree->branch[0]);
  kill_tree(tree->branch[1]);
  delete tree;
}

void iSilo::kill_huffman(struct s_huffman *h) {
  if (h == NULL) {
    return;
  }
  kill_tree(h->tree);
  delete h->code;
  delete h->size;
  delete h;
}

int iSilo::read_size(struct s_huffman *prev, struct s_huffman *h) {
  u_int32_t *j;
  u_int32_t i, n;
  int s_ok = 0, ls = 0;
  
  for (i = 0;i < h->num;) {
    j = huffman_get(prev);
    if (j == NULL)
      return(-1);
    switch(*j) {
    case HM_MEDIUM: 
      n = get_swapped(3) + 3; /* n bytes of 0 */ 
      memset(h->size + i, 0, n); 
      i += n;
      break;
    case HM_LONG:
      n = get_swapped(7) + 11; /* n bytes of 0 */
      memset(h->size + i, 0, n); 
      i += n;
      break;
    case HM_SHORT:
      if (!s_ok) {
	return(-1);
      }
      n = get_swapped(2) + 3; /* n+1 bytes of ls */
      memset(h->size + i, ls, n); 
      i += n;
      break;
    default:
      h->size[i] = *j;
      ls = *j;
      i++;
      break;
    }
    if ((*j == HM_LONG) || (*j == HM_MEDIUM)) {
      s_ok = 0;
    } else {
      s_ok = 1;
    }
  }
  return(0);
}

void iSilo::mymemcpy(u_int8_t *dst, u_int8_t *src, u_int32_t num) {
  u_int32_t i;
  for (i = 0; i < num; i++) {
    dst[i] = src[i];
  }
}
/* return size or -1 for error */

int iSilo::read_tree(struct s_huffman *prev, struct s_huffman *curr) {
  if (read_size(prev, curr) == -1)
    return(-1);
  if (size2code(curr) == -1) 
    return(-1);
  if (code2tree(curr) == -1)
    return(-1);
  return(0);
}
bool iSilo::reset_trees()
{
  get_bits(0); /* flush buffer */
  /* This is a Table record so we reload the tables */
  kill_huffman(lz);
  kill_huffman(text);
  kill_huffman(master);
    
  master = huffman_create(19);
  text = huffman_create(get_swapped(5) + 257);
  lz = huffman_create(get_swapped(5) + 1);
  int rdmax = get_swapped(4) + 4;
  
  for (int i = 0; i < rdmax; i++) 
    {
      master->size[rodata[i]] = get_swapped(3);
    }
    
  if (size2code(master) == -1) {
    qDebug("size2code(master) error: size-table is incompatible");
    return false;
  }
    
  code2tree(master);
    
  if (read_tree(master, text) == -1) {
    qDebug("read_tree() failed (format incorrect?)");
    return false;
  }
    
  if (read_tree(master, lz) == -1) {
    qDebug("read_tree() failed (format incorrect?)");
    return false;;
  }
  return true;
}
u_int32_t iSilo::get_bits(int num) {
  int i, r;
  u_int32_t result = 0;

  if (num == 0) {
    pos = 0;
    return(0);
  }

  for (i = 0; i < num; i++) {
    if (pos == 0) {
    unsuspend();
      r = fread(buf, sizeof(u_int32_t), 256, fin);
      if (r <= 0) {
	qDebug("ERROR: Unexpected end of file");
	exit(-1); /* FIXME */
      }
      pos = 32*256;
    }

    pos--;
    result <<= 1;
    result |= (ntohl(buf[255 - (pos/32)]) >> (31-(pos % 32))) & 1;
  }  
  return(result);
}
u_int32_t iSilo::get_swapped(int num) { 
  return(swap_bits(get_bits(num),num));
}
int iSilo::read_text() {
  u_int32_t *j;
  u_int32_t k, l, bp, idx;
  for (bp = 0; bp < buffer_size;) {
    j = huffman_get(text);
    if (j == NULL) 
      return(-1);
    if (*j == 256) {
      break;
    }
    if (*j >= 257) {
      idx = *j - 257;
      k = rsize_min[idx];
      if (rsize_delta[idx] != 0) {
	k += get_swapped(rsize_delta[idx]);
      }
      j = huffman_get(lz);
      if (j == NULL)
	return(-1);
      l = rpos_min[*j];
      if (rpos_delta[*j] != 0) {
	l += get_swapped(rpos_delta[*j]);
      }
      if (k <= l) {
	memcpy(buffer + bp, buffer + bp - l, k);
      } else {
	mymemcpy(buffer + bp, buffer + bp - l, k);
      } 
      bp += k;
    } else {
      buffer[bp] = *j;
      bp++;
    }
  }
  return(bp);
}
u_int32_t iSilo::getreccode()
{
  u_int32_t  offset, rec_code;
  gotorecordnumber(cur_rec);
  if (fread(&rec_code, sizeof(rec_code), 1, fin) != 1)
    {
      qDebug("short read");
      return 0xff;
    }
  return rec_code;
}
bool iSilo::process_record()
{
  u_int32_t rec_code = getreccode();
  if (((rec_code & 0xFF) == 0) && (cur_rec != 0))
    {
      return false;
    }
  else
    {
      /* process text */
      if ((cur_rec % 9) == 1)
	{
	  bsize = 0;
	  if (!reset_trees()) return false;
	  cur_rec++;
	  rec_code = getreccode();
	  if (((rec_code & 0xFF) == 0) && (cur_rec != 0))
	    {
	      return false;
	    }
	}
      if (cur_rec != 0)
	{
	  /* This is a Data record so we decode text using the table */
	  get_bits(0);
	  bsize = read_text();
	} /* end of text processing */
    }    
  return true;
}
iSilo::~iSilo()
{
  kill_huffman(master);
  kill_huffman(lz);
  kill_huffman(text);
  if (attr != NULL)
    {
      delete [] attr;
      attr = NULL;
    }
}
int iSilo::getch()
{
  if (filepos >= textsize)
    {
      //      qDebug("File end at record %u", cur_rec);
      return EOF;
    }
  if (current_pos >= bsize)
    {
      cur_rec++;
      if (!process_record()) return EOF;
      current_pos = 0;
    }
  filepos++;
  while (attr != NULL && filepos > attr[current_attr].offset)
    {
      mystyle.unset();
      u_int16_t a = attr[current_attr].value;
      //      qDebug("Handling attr:%x at pos:%u", a, filepos);
      if ((a & 0x1) != 0)
	{
	  mystyle.setBold();
	}
      if ((a & 0xe) != 0)
	{
	  qDebug("Unknown attribute:%x (%x)", a & 0xe, a);
	}
      if ((a & 0x10) != 0)
	{
	  mystyle.setCentreJustify();
	}
      else if ((a & 0x20) != 0)
	{
	  mystyle.setRightJustify();
	}
      else
	{
	  mystyle.setLeftJustify();
	}
      if ((a & 0x40) != 0)
	{
	  qDebug("Unknown attribute:%x (%x)", a & 0x40, a);
	}
      if ((a & 0x80) != 0)
	{
	  mystyle.setLeftMargin(10);
	  mystyle.setRightMargin(10);
	}
      switch (a & 0xf00)
	{
	case 0x300:
	  mystyle.setFontSize(0);
	  break;
	case 0xd00:
	  mystyle.setFontSize(1);
	  break;
	case 0xe00:
	  mystyle.setFontSize(2);
	  break;
	case 0xf00:
	  mystyle.setFontSize(3);
	  break;
	default:
	  mystyle.setFontSize(0);
	  qDebug("Not sure of font size:%x (%x)", a & 0xf00, a);
	  break;
	}
      if ((a & 0x1000) != 0)
	{
	  mystyle.setMono();
	}
      if ((a & 0x2000) != 0)
	{
	  mystyle.setItalic();
	}
      if ((a & 0x4000) != 0)
	{
	  qDebug("Unknown attribute:%x (%x)", a & 0x4000, a);
	}
      if ((a & 0x8000) != 0)
	{
	  mystyle.setUnderline();
	}
      current_attr++;
      if (current_attr >= attr_num)
	{
	  attr_rec++;
	  read_attr();
	}
    }
  return buffer[current_pos++];
}

void iSilo::getch(tchar& ch, CStyle& sty, unsigned long& pos)
{
  pos = filepos;
  ch = getch();
  sty = mystyle;
}

int iSilo::OpenFile(const char* src)
{
  struct stat _stat;
  stat(src,&_stat);
  filesize = _stat.st_size;
  pos = 0;
  cur_rec = 0;
  buffer_size = 4096;
  current_pos = 0;
  bsize = 0;
  filepos = 0;

  if (!Cpdb::openpdbfile(src))
    {
      return -1;
    }
  if (head.creator != 0x6F476F54 //   'ToGo'
      || head.type != 0x6F476F54) // 'ToGo')
    {
      return -1;
    }
  qDebug("There is %u records in this PDB file", ntohs(head.recordList.numRecords));
  init_tables();
  gotorecordnumber(0);
  fread(buffer,1,12,fin);
  fread(&textsize, sizeof(textsize), 1, fin);
  textsize = ntohl(textsize);
  fread(buffer,1,4,fin);
  fread(&attr_start,sizeof(attr_start),1,fin);
  attr_start = ntohs(attr_start);
  fread(buffer,1,2,fin);
  fread(&attr_end,sizeof(attr_end),1,fin);
  attr_end = ntohs(attr_end);
  attr_rec = attr_start;
  attr = NULL;
  pos_hi = 0xffff;
  last_pos = 0xffff;
  last_value = 0xffff;
  read_attr();
  return 0;
}

void iSilo::read_attr()
{
  //  qDebug("read_attr:<%u, %u, %u>", attr_rec, attr_start, attr_end);
  current_attr = 0;
  if (attr != NULL)
    {
      delete [] attr;
      attr = NULL;
    }
  if (attr_rec >= attr_start && attr_rec < attr_end)
    {
      gotorecordnumber(attr_rec);
      fread(buffer, 1, 4, fin);
      fread(&attr_num, sizeof(attr_num), 1, fin);
      attr_num = ntohs(attr_num)+1;
      attr = new s_attrib[attr_num];
      for (int j = 0; j < attr_num; j++)
	{
	  fread(&attr[j].offset, 2, 1, fin);
	  attr[j].offset = htons(attr[j].offset);
	}
#ifdef _WINDOWS
      for (j = 0;  j < attr_num; j++)
#else
      for (int j = 0;  j < attr_num; j++)
#endif
	  {
	  fread(&attr[j].value, 2, 1, fin);
	  if (attr[j].offset < last_pos)
	    {
	      pos_hi++;
	    }

	  if ((attr[j].offset == last_pos) && (attr[j].value == last_value))
	    {
	      pos_hi++;
	    }

	  last_pos = attr[j].offset;
	  attr[j].offset |= ((u_int32_t)pos_hi) << 16;
	  last_value = attr[j].value;
	}
      current_attr = 0;
      //      last_value = attr[attr_num-1].value;
      //      qDebug("Next attr:%u (%u)", attr[current_attr].offset, filepos);
      //      qDebug("Next attr:%x (%x)", attr[current_attr].offset, filepos);
    }
}

void iSilo::locate(unsigned int n)
{
  //  qDebug("Locating %u", n);
  if (n >= textsize) n = 0;
  pos = 0;
  buffer_size = 4096;
  current_pos = 0;
  bsize = 0;
  /* Brute force
  cur_rec = 0;
  filepos = 0;
  */ // Fast
  filepos = n - n % (8*buffer_size);
  cur_rec = 9*(n/(8*buffer_size));
  // End of fast
  pos_hi = 0xffff;
  last_pos = 0xffff;
  last_value = 0xffff;
  attr_rec = attr_start;
  read_attr();
  // While loop added for fast locate
  while (attr != NULL && attr[attr_num-1].offset < filepos)
    {
      attr_rec++;
      read_attr();
    }

  while (filepos < n)
    {
      getch();
    }
}
/*
      if (!process_record()) return EOF;
      current_pos = 0;
*/

QString iSilo::about()
{
  return QString("iSilo codec (c) Tim Wentford, based on code supplied by an anonymous donor");
}

extern "C"
{
  CExpander* newcodec() { return new iSilo; }
}
