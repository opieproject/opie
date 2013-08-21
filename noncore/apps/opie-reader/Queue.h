#ifndef __QUEUE_H
#define __QUEUE_H

#include "my_list.h"

template<class T>
class CQueue : public CList<T>
{
 public:
  bool empty() { return (CList<T>::front == NULL); }
  void push(const T& t) { this->push_back(t); }
};
#endif
