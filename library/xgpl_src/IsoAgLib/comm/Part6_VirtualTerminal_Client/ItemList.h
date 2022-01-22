#ifndef ITEM_LIST_H
#define ITEM_LIST_H


const long DEFAULT_LIST_CAPACITY = 200;


class List {
public:
    List(long size = DEFAULT_LIST_CAPACITY);
    List(List&);
    ~List();
    List& operator=(const List&);

    long Count() const;
    iVtObject_c* Get(long index) const;
    iVtObject_c* First() const;
    iVtObject_c* Last() const;
    bool Includes(const iVtObject_c*) const;

    void Append(const iVtObject_c*);
    void Prepend(const iVtObject_c*);

    void Remove(const iVtObject_c*);
    void RemoveLast();
    void RemoveFirst();
    void RemoveAll();

    iVtObject_c* Top() const;
    void Push(const iVtObject_c*);
    iVtObject_c* Pop();


private:
    void RemoveAt(long index);
  
private:
    iVtObject_c** _items;
    long _size; 
    long _count; 
};

#include "ItemList.cpp"

#endif /* ITEM_LIST_H */
