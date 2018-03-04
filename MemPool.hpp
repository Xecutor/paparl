#ifndef __MEMPOOL_HPP__
#define __MEMPOOL_HPP__

struct ListItem{
  ListItem* next;
};

template <int N,class T>
class MemPool{
  struct PoolPage{
    T items[N];
    PoolPage():next(0){}
    PoolPage* next;
  };
  PoolPage* lastPage;
  T* cur,*end;
  ListItem* freeItems;
public:
  MemPool()
  {
    lastPage=0;
    cur=end=0;
    freeItems=0;
  }
  ~MemPool()
  {
    PoolPage* ptr=lastPage;
    while(ptr)
    {
      lastPage=ptr->next;
      delete ptr;
      ptr=lastPage;
    }
  }
  T* alloc()
  {
    if(freeItems)
    {
      T* rv=(T*)freeItems;
      //DPRINT("free=%p, next=%p\n",freeItems,freeItems->next);
      freeItems=freeItems->next;
      return rv;
    }
    if(cur==end)
    {
      PoolPage* newPage=new PoolPage;
      newPage->next=lastPage;
      lastPage=newPage;
      cur=lastPage->items;
      end=lastPage->items+N;
    }
    return cur++;
  }
  void free(T* item)
  {
    //DPRINT("free %p\n",item);
    if(!item)return;
    ((ListItem*)item)->next=freeItems;
    freeItems=(ListItem*)item;
  }
  void reset()
  {
    freeItems=0;
    while(lastPage && lastPage->next)
    {
      lastPage=lastPage->next;
    }
    if(lastPage)
    {
      cur=lastPage->items;
      end=lastPage->items+N;
    }
  }
  int getAllocatedCount()
  {
    if(!lastPage)return 0;
    int rv=cur-lastPage->items;
    PoolPage* ptr=lastPage->next;
    while(ptr)
    {
      rv+=N;
      ptr=ptr->next;
    }
    ListItem* lst=freeItems;
    while(lst)
    {
      --rv;
      lst=lst->next;
    }
    return rv;
  }
};

#endif
