#ifndef __QUEUE_H
#define __QUEUE_H

#include "my_list.h"

template<class T>
class CQueue : public CList<T>
{
 public:
  bool empty() { return (front == NULL); }
  void push(const T& t) { push_back(t); }
  T pop()
    {
      T data = front->data;
      node* n = front;
      front = front->next;
      delete n;
      return data;
    }
};
#endif
