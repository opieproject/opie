#ifndef __HASH_H
#define __HASH_H

#include <stdlib.h>
/* Primes
1 2 3 5 7 11 13 17 19 23 29 31 37 41 43 47 53 59 61 67 71 73 79 83 89 97
101 103 107 109 113 127 131 137 139 149 151 157 163 167 173 179 181
191 193 197 199 211 223 227 229 233 239 241 251 257 263 269 271 277
281 283 293 307 311 313 317 331 337 347 349 353 359 367 373 379 383
389 397 401 409 419 421 431 433 439 443 449 457 461 463 467 479 487
491 499 503 509 521 523 541 547 557 563 569 571 577 587 593 599 601
607 613 617 619 631 641 643 647 653 659 661 673 677 683 691 701 709
719 727 733 739 743 751 757 761 769 773 787 797 809 811 821 823 827
829 839 853 857 859 863 877 881 883 887 907 911 919 929 937 941 947
953 967 971 977 983 991 997
*/

inline size_t hashfn(unsigned long l)
{
  return l*0x33f3;
}

inline size_t hash2fn(unsigned long l)
{
  return l & 7;
}
/*/
template<class K>size_t hashfn(const K& l)
{
  size_t s = 0;
  unsigned char* p = (unsigned char*)&l;
  for (int i = 0; i < sizeof(K)-1; i++)
    {
      s = 3*s+(255-p[i]);
    }
}

template<class K>size_t hash2fn(const K& l)
{
  unsigned char* p = (unsigned char*)&l;
  return 255-p[sizeof(K)-1];
}
*/
template<class K, class D>
class hashtable
{
 public:
  class iterator;
  friend class hashtable::iterator;
#ifndef _WINDOWS
  private:
#endif
  struct keydata
  {
    K key;
    D data;
    bool inuse;
    keydata() : inuse(false) {}
    keydata(K k, D d) : key(k), data(d), inuse(true) {}
  };
  keydata* table;
  size_t hshsz;
  bool notprime(size_t p)
  {
    size_t d;
    if (p % 2 == 0) return true;
    d = 3;
    while (d*d <= p)
    {
      if (p % d == 0)
	{
	  return true;
	}
      d += 2;
    }
    return false;
  }
  void resize()
    {
      size_t oldsz = hshsz;
      hshsz = (3*hshsz)/2;
      while (notprime(hshsz))
	{
	  hshsz++;
	}
      //printf("Size:%u\n", hshsz);
      keydata* oldtable = table;
      table = new keydata[hshsz];
      for (size_t i = 0; i < oldsz; i++)
	{
	  if (oldtable[i].inuse)
	    {
	      //printf("Setting %u to %u\n", oldtable[i].key, oldtable[i].data);
	      (*this)[oldtable[i].key] = oldtable[i].data;
	      //printf("Now %u is %u\n", oldtable[i].key, (*this)[oldtable[i].key]);
	    }
	}
      delete [] oldtable;
    }
  size_t findkey(const K& key) // returns -1 if can't find it
  {
    size_t hash2 = hash2fn(key)%(hshsz-1)+1;
    size_t hash = hashfn(key) % hshsz;
    size_t i = hash;
    //printf("Key:%u, hash:%u, hash2:%u\n", key, hash, hash2);
    while (table[i].inuse) // !empty
      {
	if (table[i].key == key)
	  {
	    //printf("Key %u present at %u\n", key, i);
	    return i; // but its the correct one & present
	  }
	i = (i+hash2) % hshsz;
	if (i == hash) // Table full
	  {
	    resize();
	    hash2 = hash2fn(key)%(hshsz-1)+1;
	    hash = hashfn(key) % hshsz;
	    i = hash;
	    //printf("(R)Key:%u, hash:%u, hash2:%u\n", key, hash, hash2);
	  }
      }
    //printf("Key %u absent at %u\n", key, i);
    return i; // found & absent
  }
 public:
  hashtable(int sz) : hshsz(sz)
    {
      while (notprime(hshsz))
	{
	  hshsz++;
	}
      //printf("Size:%u\n", hshsz);
      table = new keydata[hshsz];
    }
  ~hashtable()
    {
      delete [] table;
    }
  D& operator[](K k)
    {
      int i = findkey(k);
      table[i].key = k;
      table[i].inuse = true;
      return table[i].data;
    }
  class iterator
    {
      friend class hashtable;
#ifdef _WINDOWS
  public:
#endif
      unsigned int ptr;
      hashtable* tab;
      iterator(int t, hashtable* _tab) : ptr(t), tab(_tab)
	{
	  while (!tab->table[ptr].inuse && ptr < tab->hshsz)
	    {
	      ptr++;
	    }
	}
    public:
      iterator() : tab(NULL) {}
      iterator operator++()
	{
	  while (++ptr < tab->hshsz)
	    {
	      if (tab->table[ptr].inuse) return *this;
	    }
	  return *this;
	}
      iterator& operator=(const iterator& r)
	{
	  ptr = r.ptr;
	  tab = r.tab;
	  return *this;
	}
      bool operator!=(const iterator& r)
	{
	  return !((ptr == r.ptr) && (tab == r.tab));
	}
      bool operator==(const iterator& r)
	{
	  return ((ptr == r.ptr) && (tab == r.tab));
	}
      K first() { return tab->table[ptr].key; }
      D second() { return tab->table[ptr].data; }
    };
  iterator find(K k)
    {
      size_t i = findkey(k);
      return (table[i].inuse) ? iterator(i,this) :  end();
    }
  D& operator[](const iterator& i) // Never call with an invalid iterator!
    {
      return table[i.ptr].data;
    }
  iterator begin() { return iterator(0, this); }
  iterator end() { return iterator(hshsz, this); }
};
#endif
