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
    IsoAgLib::iVtObject_c* Get(long index) const;
    IsoAgLib::iVtObject_c* First() const;
    IsoAgLib::iVtObject_c* Last() const;
    bool Includes(IsoAgLib::iVtObject_c* const) const;

    void Append(IsoAgLib::iVtObject_c* const);
    void Prepend(IsoAgLib::iVtObject_c* const );

    void Remove(IsoAgLib::iVtObject_c* const);
    void RemoveLast();
    void RemoveFirst();
    void RemoveAll();

    IsoAgLib::iVtObject_c* Top() const;
    void Push(IsoAgLib::iVtObject_c* const);
    IsoAgLib::iVtObject_c* Pop();


    void RemoveAt(long index);
    IsoAgLib::iVtObject_c** _items;
    IsoAgLib::iVtObject_c*** all_items;
    long _size; 
    long _count; 
};

#endif /* ITEM_LIST_H */
