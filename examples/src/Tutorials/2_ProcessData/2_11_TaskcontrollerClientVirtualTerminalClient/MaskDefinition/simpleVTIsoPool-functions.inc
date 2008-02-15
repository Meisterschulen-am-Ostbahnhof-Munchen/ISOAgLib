void iObjectPool_simpleVTIsoPool_c::initAllObjectsOnce (SINGLETON_VEC_KEY_PARAMETER_DEF)
{
  if (b_initAllObjects) return;   // so the pointer to the ROM structures are only getting set once on initialization!
  iVtObjectimiIsoMaskupload.init (&iVtObjectimiIsoMaskupload_sROM SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA);
  iVtObjectos1.init (&iVtObjectos1_sROM SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA);
  iVtObjectos2.init (&iVtObjectos2_sROM SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA);
  iVtObjectMainMask.init (&iVtObjectMainMask_sROM SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA);
  iVtObjectos3.init (&iVtObjectos3_sROM SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA);
  iVtObjectFontAttributesNormal12x16.init (&iVtObjectFontAttributesNormal12x16_sROM SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA);
  iVtObjectFontAttributesSKM6x8.init (&iVtObjectFontAttributesSKM6x8_sROM SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA);

  #include "simpleVTIsoPool-functions-origin.inc"

  b_initAllObjects = true;
}
