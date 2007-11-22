
 #ifndef DECL_direct_iObjectPool_simpleVTIsoPool_c
 #define DECL_direct_iObjectPool_simpleVTIsoPool_c
class iObjectPool_simpleVTIsoPool_c : public IsoAgLib::iIsoTerminalObjectPool_c {
public:
  virtual void eventKeyCode (uint8_t keyActivationCode, uint16_t objId, uint16_t objIdMask, uint8_t keyCode, bool wasButton);
  /* Uncomment the following function if you want to use command-response handling! */
  //virtual void eventPointingEvent (uint16_t rui16_xPosition, uint16_t rui16_yPosition);
  virtual void eventNumericValue (uint16_t objId, uint8_t ui8_value, uint32_t ui32_value);
  virtual void eventStringValue (uint16_t rui16_objId, uint8_t rui8_length, StreamInput_c &refc_streaminput, uint8_t rui8_unparsedBytes, bool b_isFirst, bool b_isLast);
  /* Uncomment the following function if you want to use input value string on-the-fly parsing/handling! */
  //virtual void eventStringValueAbort();
  virtual void eventObjectPoolUploadedSuccessfully (bool rb_wasLanguageUpdate, int8_t ri8_languageIndex, uint16_t rui16_languageCode);
  //virtual void eventPrepareForLanguageChange (int8_t ri8_languageIndex, uint16_t rui16_languageCode);
  virtual void eventEnterSafeState ();
  /* Uncomment the following function if you want to use command-response handling! */
  //virtual void eventCommandResponse (uint8_t rui8_responseCommandError, const uint8_t rpui8_responseDataBytes[8]);
  /* Uncomment the following function if you want to use a special colour-conversion! */
  virtual uint8_t convertColour (uint8_t colourValue, uint8_t colourDepth, IsoAgLib::iVtObject_c* obj, IsoAgLib::e_vtColour whichColour);
  /* Uncomment the following function if you want to react on any incoming LANGUAGE_PGN */
  virtual void eventLanguagePgn (const localSettings_s& rrefs_localSettings);
  /* Uncomment the following function if you want to react on any incoming VT Status messages */
  //virtual void eventVtStatusMsg();
  void initAllObjectsOnce(SINGLETON_VEC_KEY_PARAMETER_DEF);
  iObjectPool_simpleVTIsoPool_c ();
};

 #endif
