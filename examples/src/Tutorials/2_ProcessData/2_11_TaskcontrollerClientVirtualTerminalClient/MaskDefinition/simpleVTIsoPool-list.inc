IsoAgLib::iVtObject_c* HUGE_MEM all_iVtObjects [] = {
 &iVtObjectimiIsoMaskupload,
 &iVtObjectMainMask,
 &iVtObjectFontAttributesSKM6x8,
 &iVtObjectFontAttributesNormal12x16,
 &iVtObjectos3,
 &iVtObjectos2,
 &iVtObjectos1
};

IsoAgLib::iVtObject_c* HUGE_MEM * all_iVtObjectLists [] = {
  all_iVtObjects,
  all_iVtObjects0,
  all_iVtObjects1,
  all_iVtObjects2,
  NULL // indicate end of list
};

iObjectPool_simpleVTIsoPool_c::iObjectPool_simpleVTIsoPool_c () : iIsoTerminalObjectPool_c (all_iVtObjectLists, 7, 0, 200, 60, 32) {};
