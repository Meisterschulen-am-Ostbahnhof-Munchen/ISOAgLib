/*
 * iVtObject_cList.h
 *
 *  Created on: 28.01.2022
 *      Author: franz
 */

#ifndef COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_IVTOBJECT_CLIST_H_
#define COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_IVTOBJECT_CLIST_H_


const long DEFAULT_LIST_CAPACITY = 200;


class iVtObject_cList {
public:
    iVtObject_cList(long size = DEFAULT_LIST_CAPACITY);
    ~iVtObject_cList();
    long Count() const;
    void Append(IsoAgLib::iVtObject_c* const);
    IsoAgLib::iVtObject_c** _items;
    IsoAgLib::iVtObject_c*** all_items;
    long _size;
    long _count; 
};

#endif /* COMPONENTS_ISOAGLIB_LIBRARY_XGPL_SRC_ISOAGLIB_COMM_PART6_VIRTUALTERMINAL_CLIENT_IVTOBJECT_CLIST_H_ */

