
 #ifndef DECL_direct_iObjectPool_simpleVTIsoPool_c
 #define DECL_direct_iObjectPool_simpleVTIsoPool_c
class iObjectPool_simpleVTIsoPool_c : public IsoAgLib::iIsoTerminalObjectPool_c {
public:
  virtual void eventKeyCode (uint8_t keyActivationCode, uint16_t objId, uint16_t objIdMask, uint8_t keyCode, bool wasButton);
  virtual void eventNumericValue (uint16_t objId, uint8_t ui8_value, uint32_t ui32_value);
  virtual void eventObjectPoolUploadedSuccessfully ();
  virtual uint8_t convertColor(uint8_t colorValue, uint8_t colorDepth);
  void initAllObjectsOnce();
  iObjectPool_simpleVTIsoPool_c ();
};

 #endif
