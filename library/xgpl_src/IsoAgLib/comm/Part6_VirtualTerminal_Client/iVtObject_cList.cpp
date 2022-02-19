/**
 * References:
 *
 *   [1] http://st-www.cs.illinois.edu/patterns/dpbook/Source.html
 */

#include <IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtincludes.h>
#include <iostream>
#include "iVtObject_cList.h"

namespace IsoAgLib
{



iVtObject_cList::iVtObject_cList(long size) : _size(size), _count(0) {
    _items = new iVtObject_c*[size];
    all_items = new iVtObject_c**[2];
    all_items[0] = _items;
    all_items[1] = nullptr;


}


iVtObject_cList::~iVtObject_cList() {
    delete[] _items;
}


long iVtObject_cList::Count() const {
    return (_count);
}


void iVtObject_cList::Append(iVtObject_c* const anItem) {
    if (_count <= _size) {
        _items[_count] = anItem;
        _count++;
    } else {
    	//Exception !!! //TODO
    }
}

} //namespace IsoAgLib









