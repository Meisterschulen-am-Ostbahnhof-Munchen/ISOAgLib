
 #ifndef DECL_direct_iObjectPool_simpleVTIsoPool_c
 #define DECL_direct_iObjectPool_simpleVTIsoPool_c
class iObjectPool_simpleVTIsoPool_c : public IsoAgLib::iIsoTerminalObjectPool_c {
public:
  virtual void eventKeyCode (uint8_t keyActivationCode, uint16_t objId, uint16_t objIdMask, uint8_t keyCode, bool wasButton);
  virtual void eventNumericValue (uint16_t objId, uint8_t ui8_value, uint32_t ui32_value);
  virtual void eventObjectPoolUploadedSuccessfully ();
  /* Uncommented the following function because I want to use a special colour-conversion! */
  virtual uint8_t convertColour (uint8_t colourValue, uint8_t colourDepth, IsoAgLib::iVtObject_c* obj, IsoAgLib::e_vtColour whichColour);
  void initAllObjectsOnce();
  iObjectPool_simpleVTIsoPool_c ();
};

 #endif
