/**
 * References:
 *
 *   [1] http://st-www.cs.illinois.edu/patterns/dpbook/Source.html
 */

#include <iostream>

List::List(long size) : _size(size), _count(0) {
    _items = new iVtObject_c*[size];
}

List::List(List &list) : _size(list._size), _count(list._count) {
	_items = list._items;
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
Item& List::Get(long index) const {
    if (_count <= _size) {
      return (_items[index]);
    } else {
      // throw RangeError exception
      return (_items[index]);
    }
}


Item& List::First() const {
    return (Get(0));
}


Item& List::Last() const {
  return (Get(Count()-1));
}


bool List::Includes(const iVtObject_c* anItem) const {
    for (long i = 0; i < Count(); i++) {
        if (_items[i] == anItem) {
            return (true);
        }
    }
    return (false);
}



void List::Append(const iVtObject_c* anItem) {
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



Item& List::Top() const {
    return (Last());
}


void List::Push(iVtObject_c* anItem) {
    Append(anItem);
}


Item& List::Pop()
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




