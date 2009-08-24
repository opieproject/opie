#include <sys/types.h>
#include <sys/stat.h>
#ifndef _WINDOWS
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#else
typedef unsigned char u_int8_t;
typedef unsigned short u_int16_t;
typedef unsigned long u_int32_t;
#endif
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include "pdb.h"

#define VERSION "deSilo version 0.4a by Clement"


struct s_tree {
  struct s_tree *branch[2];
  u_int32_t value;
};

struct s_attrib {
  u_int32_t offset;
  u_int16_t value;
};

struct s_huffman {
  struct s_tree *tree;
  u_int8_t *size;
  u_int16_t *code;
  u_int32_t num;
};

#define HM_SHORT  0x10
#define HM_MEDIUM 0x11
#define HM_LONG   0x12

class iSilo : public Cpdb
{
  CStyle mystyle;
  u_int16_t BlockSize;
  int pos;
  u_int32_t buf[256];
  struct s_huffman *master, *lz, *text;
  u_int32_t cur_rec;
  u_int8_t buffer[4096];
  u_int32_t buffer_size;
  u_int16_t current_pos, bsize;
  u_int32_t filepos, filesize, textsize;
  u_int16_t attr_start, attr_end, attr_num, current_attr, attr_rec;
  u_int16_t pos_hi;
  u_int16_t last_pos, last_value;


  s_attrib* attr;


  int code2tree(struct s_huffman *h);
  u_int32_t swap_bits(u_int32_t n, int num);
  u_int32_t *huffman_get(struct s_huffman *h);
  int size2code(struct s_huffman *h);
  void read_attr();
  struct s_huffman *huffman_create(u_int32_t num);
  void kill_tree(struct s_tree *tree);
  void kill_huffman(struct s_huffman *h);
  int read_size(struct s_huffman *prev, struct s_huffman *h);
  void mymemcpy(u_int8_t *dst, u_int8_t *src, u_int32_t num);
  int read_tree(struct s_huffman *prev, struct s_huffman *curr);
  bool reset_trees();
  u_int32_t get_bits(int num);
  u_int32_t get_swapped(int num);
  int read_text();
  u_int32_t getreccode();
  bool process_record();
public:
  iSilo() : pos(0), master(NULL), lz(NULL), text(NULL), cur_rec(0), buffer_size(4096), current_pos(0), bsize(0), BlockSize(4096)
  {
  }
  ~iSilo();
  int getch();
  void getch(tchar& ch, CStyle& sty, unsigned long& pos);
  MarkupType PreferredMarkup()
  {
    return cNONE;
  }
  void sizes(unsigned long& _file, unsigned long& _text)
  {
    _file = filesize;
    _text = textsize;
  }
  bool hasrandomaccess() { return false; }
  int OpenFile(const char* src);
  unsigned int locate()
  {
    return filepos;
  }
  void locate(unsigned int n);
  QString about();
};
