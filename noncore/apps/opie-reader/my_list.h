#ifndef __MY_LIST_H
#define __MY_LIST_H

template<class T>
class CList
{
  struct node
  {
    T data;
    node* next;
    node(T _data, node* _next = NULL) : data(_data), next(_next) {}
    node() : next(NULL) {};
  };
 protected:
  node* front;
  node* back;
 public:
  CList() : front(NULL), back(NULL) {}
  ~CList()
    {
      if (front != NULL)
	{
	  while (front != NULL)
	    {
	      node *p = front;
	      front = p->next;
	      delete p;
	    }
	}
    }
  T* operator[](int n)
    {
      node* current = front;
      while (n-- > 0)
	{
	  if ((current = current->next) == NULL)
	    return NULL;
	}
      return &(current->data);
    }
  void push_front(const T& t)
    {
      node* n = new node(t,front);
      if (front == NULL)
	{
	  front = back = n;
	}
      else
	front = n;
    }
  void push_back(const T& t)
    {
      node* n = new node(t);
      if (front == NULL)
	{
	  front = back = n;
	}
      else
	{
	  back->next = n;
	  back = n;
	}
    }
  void erase(unsigned int n)
    {
      node* p = front;
      node* last = front;
      while (n-- > 0)
	{
	  last = p;
	  p = p->next;
	  if (p == NULL) return;
	}
      if (p == front)
	{
	  front = p->next;
	}
      else
	{
	  last->next = p->next;
	}
      if (p == back)
	{
	  back = last;
	}
      delete p;
    }
  void sort()
    {
      int i,j,inc,n;
      T v;
      T* item;
      node* t;
      t = front;
      n = 0;
      while (t != NULL)
	{
	  n++;
	  t = t->next;
	}
      if (n >= 2)
      {
        item = new T[n];
        i = 0;
        t = front;
        for (t = front, i = 0; t != NULL; t = t->next, i++)
	  {
	    item[i] = t->data;
          }

        for (inc = 1; inc <= n; inc = 3*inc+1);

        do
          {
            inc /= 3;
	    for (i = inc; i < n; i++)
	      {
	        v = item[i];
                for (j = i; v < item[j-inc] && j >= inc; j -= inc)
	          {
		    item[j] = item[j-inc];
		  }
                item[j] = v;
	      }
	  }
        while (inc > 1);
        for (t = front, i = 0; t != NULL; t = t->next, i++)
	  {
	    t->data = item[i];
	  }
      //      back = *(item[n-1]);
        delete [] item;
      }
    }
  class iterator
  {
    node* current;
  public:
    iterator(node* _c) : current(_c) {}
    iterator& operator++()
    {
      current = current->next;
      return *this;
    }
    iterator& operator++(int)
    {
      current = current->next;
      return *this;
    }
    T operator*()
      {
	return current->data;
      }
    T* operator->()
      {
	return &(current->data);
      }
    bool operator!=(iterator t)
    {
      return (current != t.current);
    }
  };
  iterator begin()
    {
      return iterator(front);
    }
  iterator end()
    {
      return iterator(NULL);
    }
};
#endif
