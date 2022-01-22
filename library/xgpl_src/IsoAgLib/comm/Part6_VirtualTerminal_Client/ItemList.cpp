/**
 * References:
 *
 *   [1] http://st-www.cs.illinois.edu/patterns/dpbook/Source.html
 */

#include <IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtincludes.h>
#include <iostream>
#include "ItemList.h"

using namespace IsoAgLib;

List::List(long size) : _size(size), _count(0) {
    _items = new iVtObject_c*[size];
    all_items = new iVtObject_c**[2];
    all_items[0] = _items;
    all_items[1] = nullptr;


}

List::List(List &list) : _size(list._size), _count(list._count) {
	_items = list._items;
	all_items = list.all_items;
}


List::~List() {
    delete[] _items;
}

List& List::operator =(const List &constList) {
    if (this != &constList) // no self-assignment.
    {
    	_size = constList._size;
    	_count = constList._count;
    	_items = constList._items;
    }
    return (*this);
}

long List::Count() const {
    return (_count);
}

// TODO:
//   * solve warning 'control reaches end of non-void function'.
//   * shouldn't the test be if (index < _count) ???
iVtObject_c* List::Get(long index) const {
    if (_count <= _size) {
      return (_items[index]);
    } else {
      // throw RangeError exception
      return (_items[index]);
    }
}


iVtObject_c* List::First() const {
    return (Get(0));
}


iVtObject_c* List::Last() const {
  return (Get(Count()-1));
}


bool List::Includes(iVtObject_c* const anItem) const {
    for (long i = 0; i < Count(); i++) {
        if (_items[i] == anItem) {
            return (true);
        }
    }
    return (false);
}



void List::Append(iVtObject_c* const anItem) {
    if (_count <= _size) {
        _items[_count] = anItem;
        _count++;
    }
}


void List::Prepend(iVtObject_c* anItem) {
    if (_count <= _size) {
        for (long i = _count-1; i >= 0; i--)
            _items[i+1] = _items[i];
        _items[0] = anItem;
        _count++;
    }
}


void List::Remove(iVtObject_c* anItem) {
    for (long i = 0; i < Count(); i++) {
        if (_items[i] == anItem) {
            RemoveAt(i);
        }
    }
}


void List::RemoveLast() {
    RemoveAt(Count()-1);
}


void List::RemoveFirst() {
    RemoveAt(0);
}


void List::RemoveAll() {
    _count = 0;
}



iVtObject_c* List::Top() const {
    return (Last());
}


void List::Push(iVtObject_c* const anItem) {
    Append(anItem);
}


iVtObject_c* List::Pop()
{
	iVtObject_c* top = Last();
    RemoveLast();
    return (top);
}




void List::RemoveAt(long index) {
    if (index < 0 || index >= Count())
        std::cout << "Throw RangeException\n";
    for (long i = index; i < Count()-1; i++)
        _items[i] = _items[i+1];
    _count--;
}




