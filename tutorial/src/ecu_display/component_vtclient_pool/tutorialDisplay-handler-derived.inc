#ifndef DECL_derived_iObjectPool_tutorialDisplay_c
#define DECL_derived_iObjectPool_tutorialDisplay_c

// forward declaration
extern IsoAgLib::iVtObject_c* HUGE_MEM * all_iVtObjectLists [];

class iObjectPool_tutorialDisplay_c : public IsoAgLib::iIsoTerminalObjectPool_c {
public:
  void initAllObjectsOnce(SINGLETON_VEC_KEY_PARAMETER_DEF);
  iObjectPool_tutorialDisplay_c() : iIsoTerminalObjectPool_c (all_iVtObjectLists, 130, 4, 200, 60, 47) {}

};

#endif
