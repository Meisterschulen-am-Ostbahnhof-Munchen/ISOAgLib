IsoAgLib::iVtObject_c* HUGE_MEM all_iVtObjects [] = {
 &iVtObjectimiIsoMaskupload,
 &iVtObjectMacro_1,
 &iVtObjectMacro_2,
 &iVtObjectMainMask,
 &iVtObjectValMiles,
 &iVtObjectsoftkeysAlarm,
 &iVtObjectUnnamed38,
 &iVtObjectUnnamed37,
 &iVtObjectUnnamed36,
 &iVtObjectalarm1contentscontainer,
 &iVtObjectForbiddenAlarmMask,
 &iVtObjectUnnamed35,
 &iVtObjectUnnamed34,
 &iVtObjectUnnamed33,
 &iVtObjectUnnamed32,
 &iVtObjectUnnamed31,
 &iVtObjectUnnamed30,
 &iVtObjectUnnamed29,
 &iVtObjectAnotherMask,
 &iVtObjectdMeter,
 &iVtObjectdPolygon,
 &iVtObjectdEllipse,
 &iVtObjectFillAttributes,
 &iVtObjectLineAttributes_Normal,
 &iVtObjectUnnamed28,
 &iVtObjectKeyChangeLogoPosition,
 &iVtObjectUnnamed27,
 &iVtObjectKeyUpdatePool,
 &iVtObjectUnnamed26,
 &iVtObjectKeyChangeFill,
 &iVtObjectUnnamed25,
 &iVtObjectKeyForbidden,
 &iVtObjectUnnamed24,
 &iVtObjectUnnamed23,
 &iVtObjectKeyTransCol,
 &iVtObjectUnnamed22,
 &iVtObjectUnnamed21,
 &iVtObjectKeyChangeCol,
 &iVtObjectUnnamed20,
 &iVtObjectKeyChangeLang,
 &iVtObjectUnnamed19,
 &iVtObjectKeyLogo,
 &iVtObjectUnnamed18,
 &iVtObjectKeyMove,
 &iVtObjectsoftkeys1,
 &iVtObjectOSresonible,
 &iVtObjectISdriver,
 &iVtObjectresonible,
 &iVtObjectdriver,
 &iVtObjectOSlanguage,
 &iVtObjectvtLang,
 &iVtObjectUnnamed17,
 &iVtObjectKeyLessAccel,
 &iVtObjectUnnamed16,
 &iVtObjectKeyMoreAccel,
 &iVtObjectLikeageOP,
 &iVtObjectInputLikeage,
 &iVtObjectUnnamed11,
 &iVtObjectUnnamed10,
 &iVtObjectUnnamed15,
 &iVtObjectUnnamed14,
 &iVtObjectUnnamed13,
 &iVtObjectUnnamed12,
 &iVtObjectInputListTypFakeMiles,
 &iVtObjectChangeAwayPointer,
 &iVtObjectOpsFakeMileage,
 &iVtObjectAccelArchedBarGraph,
 &iVtObjectColOS,
 &iVtObjectColLabel,
 &iVtObjectUnnamed9,
 &iVtObjectUnnamed8,
 &iVtObjectUnnamed7,
 &iVtObjectUnnamed6,
 &iVtObjectUnnamed5,
 &iVtObjectspeedLabel,
 &iVtObjectUnnamed4,
 &iVtObjectmask1contentscontainer,
 &iVtObjectInputMiles,
 &iVtObjectKeyReset,
 &iVtObjectleine,
 &iVtObjectBigLogo,
 &iVtObjectUnnamed3,
 &iVtObjectcontainerInAllMasks,
 &iVtObjectLikeItNot,
 &iVtObjectLikeIt,
 &iVtObjectStrWhite,
 &iVtObjectStrBlack,
 &iVtObjectStrMagenta,
 &iVtObjectStrCyan,
 &iVtObjectStrYellow,
 &iVtObjectValAccel,
 &iVtObjectValSpeed,
 &iVtObjectFontAttributesNormal6x8_Button60x30,
 &iVtObjectLineAttributesSKM,
 &iVtObjectFontAttributesSKM12x16,
 &iVtObjectFontAttributesSKM8x8,
 &iVtObjectFontAttributesSKM6x8,
 &iVtObjectLineAttributesDotted,
 &iVtObjectLineAttributesNormal,
 &iVtObjectFontAttributesNormal24x32,
 &iVtObjectFontAttributesNormal16x24,
 &iVtObjectFontAttributesNormal16x16,
 &iVtObjectFontAttributesNormal12x16,
 &iVtObjectFontAttributesNormal8x8,
 &iVtObjectFontAttributesNormal6x8B,
 &iVtObjectFontAttributesNormal6x8R,
 &iVtObjectFontAttributesNormal8x12,
 &iVtObjectFontAttributesNormal6x8,
 &iVtObjectUnnamed2,
 &iVtObjectUnnamed1
};

IsoAgLib::iVtObject_c* HUGE_MEM * all_iVtObjectLists [] = {
  all_iVtObjects,
  all_iVtObjects0,
  all_iVtObjects1,
  all_iVtObjects2,
  NULL // indicate end of list
};

iObjectPool_simpleVTIsoPool_c::iObjectPool_simpleVTIsoPool_c () : iIsoTerminalObjectPool_c (all_iVtObjectLists, 110, 6, 200, 60, 32) {};