#ifndef __INTRSORTEDLIST_HPP__
#define __INTRSORTEDLIST_HPP__

#include <sys/types.h>


struct IntrSortedListEmptyStruct{};

template <class T,class Base=IntrSortedListEmptyStruct>
struct IntrSortedListNodeBase:public Base{
  T* islPrevNode;
  T* islNextNode;
};

template <class T>
class IntrSortedList{
public:
  IntrSortedList():head(0),tail(0),count(0)
  {
  }
  void Clear()
  {
    head=0;
		tail=0;
    count=0;
  }
  
  T* getHead()
  {
    return head;
  }

  void Push(T* value)
  {
    if(!head)
    {
      head=value;
      head->islNextNode=0;
      head->islPrevNode=0;
      tail=head;
      count=1;
      return;
    }
    Node* ptr=head;
    while(ptr && ptr->lessThan(value))
    {
      ptr=ptr->islNextNode;
    }
    if(ptr)
    {
      if(ptr==head)
      {
        value->islPrevNode=0;
        value->islNextNode=head;
        head->islPrevNode=value;
        head=value;
      }else
      {
        ptr->islPrevNode->islNextNode=value;
        value->islPrevNode=ptr->islPrevNode;
        value->islNextNode=ptr;
        ptr->islPrevNode=value;
      }
    }else
    {
      tail->islNextNode=value;
      value->islNextNode=0;
      value->islPrevNode=tail;
      tail=value;
    }
    count++;
  }

  T* Pop()
  {
    if(!head)return 0;
    T* rv=head;
    head=head->islNextNode;
    if(head)
    {
      head->islPrevNode=0;
    }
    count--;
    return rv;
  }

  void Delete(T* value)
  {
    if(value==head)
    {
      head=head->islNextNode;
      if(head)
      {
        head->islPrevNode=0;
      }
    }else if(value==tail)
    {
      tail=tail->islPrevNode;
      tail->islNextNode=0;
    }else
    {
      value->islPrevNode->islNextNode=value->islNextNode;
      value->islNextNode->islPrevNode=value->islPrevNode;
    }
    count--;
  }

  size_t getSize()const
  {
    return count;
  }
  
  bool isEmpty()const
  {
    return count==0;
  }


  void Update(T* value)
  {
    if(count==1)return;
    if(count==2)
    {
      if(value==head)
      {
        if(tail->lessThan(head))
        {
          head=tail;
          tail=value;
          head->islNextNode=tail;
          head->islPrevNode=0;
          tail->islNextNode=0;
          tail->islPrevNode=head;
        }
      }else
      {
        if(tail->lessThan(head))
        {
          tail=head;
          head=value;
          head->islNextNode=tail;
          head->islPrevNode=0;
          tail->islNextNode=0;
          tail->islPrevNode=head;
        }
      }
      return;
    }
    if(value==head || (value->islNextNode && value->islNextNode->lessThan(value)))
    {
      Node* ptr=value->islNextNode;
      while(ptr && ptr->lessThan(value))
      {
        ptr=ptr->islNextNode;
      }
      if(value==head && ptr==head->islNextNode)
      {
        return;
      }
      if(value==head)
      {
        head=head->islNextNode;
        head->islPrevNode=0;
      }else
      {
        value->islPrevNode->islNextNode=value->islNextNode;
        value->islNextNode->islPrevNode=value->islPrevNode;
      }
      
      if(ptr)
      {
        value->islNextNode=ptr;
        value->islPrevNode=ptr->islPrevNode;
        ptr->islPrevNode->islNextNode=value;
        ptr->islPrevNode=value;
      }else
      {
        value->islNextNode=0;
        value->islPrevNode=tail;
        tail->islNextNode=value;
        tail=value;
      }
      return;
    }
    if(value==tail || (value->islPrevNode && value->lessThan(value->islPrevNode)))
    {
      Node* ptr=value->islPrevNode;
      while(ptr && value->lessThan(ptr))
      {
        ptr=ptr->islPrevNode;
      }
      if(value==tail && ptr==tail->islPrevNode)
      {
        return;
      }
      if(value==tail)
      {
        tail=tail->islPrevNode;
        tail->islNextNode=0;
      }else
      {
        value->islPrevNode->islNextNode=value->islNextNode;
        value->islNextNode->islPrevNode=value->islPrevNode;
      }
      
      if(ptr && ptr->islPrevNode)
      {
        value->islNextNode=ptr;
        value->islPrevNode=ptr->islPrevNode;
        ptr->islPrevNode->islNextNode=value;
        ptr->islPrevNode=value;
      }else
      {
        value->islNextNode=head;
        head->islPrevNode=value;
        value->islPrevNode=0;
        head=value;
      }
      return;
    }
  }


protected:
  typedef T Node;
  Node* head;
  Node* tail;
  size_t count;
};

#endif
