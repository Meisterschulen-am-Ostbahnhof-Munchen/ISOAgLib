/**
 * References:
 *
 *   [1] http://st-www.cs.illinois.edu/patterns/dpbook/Source.html
 */

#include <IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtincludes.h>
#include <iostream>
#include "iVtObject_cList.h"
#include <stdexcept>

using std::runtime_error;

namespace IsoAgLib
{



iVtObject_cList::iVtObject_cList(long size)
		: _size(size)
		, _count(1) //we want to have 1 here, to reserve 0 for WorkingSet. !
{
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


void iVtObject_cList::AppendWs(iVtObjectWorkingSet_c* const anWsItem) {
	//WorkingSet MUST be on Place 0
    _items[0] = anWsItem;
}


void iVtObject_cList::Append(iVtObject_c* const anItem) {
    if (_count <= _size) {
        _items[_count] = anItem;
        _count++;
    } else {
    	throw runtime_error("_items Array is full !");
    }
}

} //namespace IsoAgLib









