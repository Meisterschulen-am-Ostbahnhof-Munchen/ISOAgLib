void iObjectPool_simpleVTIsoPool_c::initAllObjectsOnce (SINGLETON_VEC_KEY_PARAMETER_DEF)
{
  if (b_initAllObjects) return;   // so the pointer to the ROM structures are only getting set once on initialization!
  int i_objCount = 110;
  int i_listIndex = 0;
  IsoAgLib::iVtObject_c::iVtObject_s* HUGE_MEM * pps_sROMs = all_sROMs;
  do 
  {
    IsoAgLib::iVtObject_c* HUGE_MEM * pc_objList = all_iVtObjectLists [i_listIndex];
    while (i_objCount)
    {
      ((__IsoAgLib::vtObject_c*)(*pc_objList))->init (*pps_sROMs);
      ++pc_objList;
      ++pps_sROMs;
      --i_objCount;
    }
    i_objCount = 6;
    ++i_listIndex;
  } while (all_iVtObjectLists[i_listIndex]);

  #include "simpleVTIsoPool-functions-origin.inc"

  b_initAllObjects = true;
}
