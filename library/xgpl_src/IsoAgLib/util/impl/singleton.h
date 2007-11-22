#ifndef SINGLETON_H_
#define SINGLETON_H_

#include <Application_Config/isoaglib_config.h>

#include "../compiler_adaptation.h"

#if defined(OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED) && defined( __GNUC__ ) && __GNUC__ >= 4
  #include <ext/malloc_allocator.h>
#endif

#if defined(PRT_INSTANCE_CNT) && (PRT_INSTANCE_CNT != 1 )
  /** the macro SINGLETON allows to define classes independent from the value
    * of PRT_INSTANCE_CNT, so that the compiler selects the appropriate base class during
    * compile time
    * this variant is used by the compiler, if more than one BUSes have to be managed by
    * the IsoAgLib
    */
  #define SINGLETON( CLASS ) SingletonVec<CLASS,PRT_INSTANCE_CNT>
  /** the macro SINGLETON_DERIVED allows to define classes independent from the value
    * of PRT_INSTANCE_CNT, so that the compiler selects the appropriate base class during
    * compile time
    * this variant is used by the compiler, if more than one BUSes have to be managed by
    * the IsoAgLib
    * Difference to SINGLETON is the fact, that this version allows to derive the singleton pattern using class from
    * another class. This is important to avoid multiple inheritance.
    */
  #define SINGLETON_DERIVED( CLASS, BASE ) SingletonDerivedVec<CLASS,BASE,PRT_INSTANCE_CNT>

  /** the macro SINGLETON_CLIENT1 allows to define a singleton, which allows ONE (aka 1)
    * client class to register pointers to each instance (the register function has to be called
    * during execution of constructor, whereas the unregister function has to be called during
    * execution of destructor. This macro allows to define specific classes independent from
    * the value of PRT_INSTANCE_CNT. With the help of the macro autoInstance(), the interaction of
    * instances of different classes which belongs to the same BUS can be defined independent
    * from PRT_INSTANCE_CNT
    */
  #define SINGLETON_CLIENT1( CLASS, CLIENT1, KEY1 ) SingletonVecCont1<CLASS,PRT_INSTANCE_CNT,CLIENT1,KEY1>
  /** the macro SINGLETON_CLIENT1 allows to define a singleton, which allows ONE (aka 1)
    * client class to register pointers to each instance (the register function has to be called
    * during execution of constructor, whereas the unregister function has to be called during
    * execution of destructor. This macro allows to define specific classes independent from
    * the value of PRT_INSTANCE_CNT. With the help of the macro autoInstance(), the interaction of
    * instances of different classes which belongs to the same BUS can be defined independent
    * from PRT_INSTANCE_CNT
    * Difference to SINGLETON_CLIENT1 is the fact, that this version allows to derive the singleton pattern using class from
    * another class. This is important to avoid multiple inheritance.
    */
  #define SINGLETON_DERIVED_CLIENT1( CLASS, BASE, CLIENT1, KEY1 ) SingletonDerivedVecCont1<CLASS,BASE,PRT_INSTANCE_CNT,CLIENT1,KEY1>

  /** the macro SINGLETON_CLIENT2 allows to define a singleton, which allows TWO (aka 2)
    * client classes to register pointers to each instance (the register function has to be called
    * during execution of constructor, whereas the unregister function has to be called during
    * execution of destructor. This macro allows to define specific classes independent from
    * the value of PRT_INSTANCE_CNT. With the help of the macro autoInstance(), the interaction of
    * instances of different classes which belongs to the same BUS can be defined independent
    * from PRT_INSTANCE_CNT
    */
  #define SINGLETON_CLIENT2( CLASS, CLIENT1, KEY1, CLIENT2, KEY2 ) \
    SingletonVecCont2<CLASS,PRT_INSTANCE_CNT,CLIENT1,KEY1,CLIENT2,KEY2>
  /** the macro SINGLETON_CLIENT2 allows to define a singleton, which allows TWO (aka 2)
    * client classes to register pointers to each instance (the register function has to be called
    * during execution of constructor, whereas the unregister function has to be called during
    * execution of destructor. This macro allows to define specific classes independent from
    * the value of PRT_INSTANCE_CNT. With the help of the macro autoInstance(), the interaction of
    * instances of different classes which belongs to the same BUS can be defined independent
    * from PRT_INSTANCE_CNT
    * Difference to SINGLETON_CLIENT2 is the fact, that this version allows to derive the singleton pattern using class from
    * another class. This is important to avoid multiple inheritance.
    */
  #define SINGLETON_DERIVED_CLIENT2( CLASS, BASE, CLIENT1, KEY1, CLIENT2, KEY2 ) \
    SingletonDerivedVecCont2<CLASS,BASE,PRT_INSTANCE_CNT,CLIENT1,KEY1,CLIENT2,KEY2>

  /** the macro SINGLETON_CLIENT3 allows to define a singleton, which allows THREE (aka 3)
    * client classes to register pointers to each instance (the register function has to be called
    * during execution of constructor, whereas the unregister function has to be called during
    * execution of destructor. This macro allows to define specific classes independent from
    * the value of PRT_INSTANCE_CNT. With the help of the macro autoInstance(), the interaction of
    * instances of different classes which belongs to the same BUS can be defined independent
    * from PRT_INSTANCE_CNT
    */
  #define SINGLETON_CLIENT3( CLASS, CLIENT1, KEY1, CLIENT2, KEY2, CLIENT3, KEY3 ) \
    SingletonVecCont3<CLASS,PRT_INSTANCE_CNT,CLIENT1,KEY1,CLIENT2,KEY2,CLIENT3,KEY3>
  /** the macro SINGLETON_CLIENT3 allows to define a singleton, which allows THREE (aka 3)
    * client classes to register pointers to each instance (the register function has to be called
    * during execution of constructor, whereas the unregister function has to be called during
    * execution of destructor. This macro allows to define specific classes independent from
    * the value of PRT_INSTANCE_CNT. With the help of the macro autoInstance(), the interaction of
    * instances of different classes which belongs to the same BUS can be defined independent
    * from PRT_INSTANCE_CNT
    * Difference to SINGLETON_CLIENT3 is the fact, that this version allows to derive the singleton pattern using class from
    * another class. This is important to avoid multiple inheritance.
    */
  #define SINGLETON_DERIVED_CLIENT3( CLASS, BASE, CLIENT1, KEY1, CLIENT2, KEY2, CLIENT3, KEY3 ) \
    SingletonDerivedVecCont3<CLASS,BASE,PRT_INSTANCE_CNT,CLIENT1,KEY1,CLIENT2,KEY2,CLIENT3,KEY3>

  /** the macro autoInstance() uses the class  value singletonVecKey
    * to get a corresponding class instance, which is delegated to the same BUS
    */
  #define autoInstance()                    instance( ClientBase::getSingletonVecKey() )

  #define SINGLETON_VEC_KEY_PARAMETER_DEF               int ai_singletonVecKey
  #define SINGLETON_VEC_KEY_PARAMETER_DEFAULT_NULL_DEF  int ai_singletonVecKey = 0
  #define SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA  , int ai_singletonVecKey
  #define SINGLETON_VEC_KEY_PARAMETER_DEFAULT_NULL_DEF_WITH_COMMA , int ai_singletonVecKey = 0
  #define SINGLETON_VEC_KEY_PARAMETER_USE               ai_singletonVecKey
  #define SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA  , ai_singletonVecKey

  #define SINGLETON_PAR_DOT_DEF(PAR)                     int getSingletonVecKey() const { return PAR.getSingletonVecKey(); }
  #define SINGLETON_PAR_ARR_DEF(PAR)                     int getSingletonVecKey() const { return PAR->getSingletonVecKey(); }
  #define SINGLETON_PAR_BASE_DEF(PAR)       int getSingletonVecKey() const { return PAR::getSingletonVecKey(); }
  #define SINGLETON_MC_DATA_ASSIGN                   mc_data.setSingletonKey( ai_singletonVecKey );
  #define SINGLETON_MEMBER_DEF               ClientBase c_clientBase; \
                                                    int getSingletonVecKey() const { return c_clientBase.getSingletonVecKey(); }
  #define SINGLETON_MEMBER_ASSIGN(PAR)     c_clientBase.setSingletonKey (PAR.c_clientBase.getSingletonVecKey());
  #define SINGLETON_MEMBER_CONSTRUCTOR    c_clientBase( ai_singletonVecKey ),
  #define SINGLETON_PARENT_CONSTRUCTOR      ClientBase( ai_singletonVecKey ),
  #define SINGLETON_VEC_KEY_INIT_CALL                   setSingletonKey( ai_singletonVecKey );

  #define SINGLETON_VEC_KEY                             getSingletonVecKey()
  #define SINGLETON_VEC_KEY_WITH_COMMA                , getSingletonVecKey()


  #define getCanInstance4Comm()             getCanInstance( getSingletonVecKey() )
  #define getSchedulerInstance4Comm()       getSchedulerInstance( getSingletonVecKey() )
  #define getTimePosGpsInstance4Comm()      getTimePosGpsInstance( getSingletonVecKey() )
  #define getTracGeneralInstance4Comm()     getTracGeneralInstance( getSingletonVecKey() )
  #define getTracMoveInstance4Comm()        getTracMoveInstance( getSingletonVecKey() )
  #define getTracLightInstance4Comm()       getTracLightInstance( getSingletonVecKey() )
  #define getTracAuxInstance4Comm()         getTracAuxInstance( getSingletonVecKey() )
  #define getTracPtoInstance4Comm()         getTracPtoInstance( getSingletonVecKey() )
  #define getIsoMonitorInstance4Comm()      getIsoMonitorInstance( getSingletonVecKey() )
  #define getProcessInstance4Comm()         getProcessInstance( getSingletonVecKey() )
  #define getMultiSendInstance4Comm()       getMultiSendInstance( getSingletonVecKey() )
  #define getIsoTerminalInstance4Comm()     getIsoTerminalInstance( getSingletonVecKey() )
  #define getMultiReceiveInstance4Comm()    getMultiReceiveInstance( getSingletonVecKey() )
  #define getIsoRequestPgnInstance4Comm()   getIsoRequestPgnInstance(getSingletonVecKey())
  #define getMaskHandlerInstance4Comm()     getMaskHandlerInstance(getSingletonVecKey())
  #define getIsoFilterManagerInstance4Comm() getIsoFilterManagerInstance(getSingletonVecKey())
  #define getMasterGuiInstance4Comm()       getMasterGuiInstance(getSingletonVecKey())
  #define getProprietaryMessageHandlerInstance4Comm() getProprietaryMessageHandlerInstance(getSingletonVecKey())

/** the class ClientBase delivers the base information, to concat client class instances
    * with the corresponding server class instance. This is realized by the single
    * attribute singletonVecKey, which is evaluated by the macro autoInstance()
    */
  class ClientBase {
   public:
    ClientBase( int ai_singletonVecKey = 0 ) : mi_singletonVecKey(ai_singletonVecKey){};
    ClientBase( const ClientBase& rc_src )
      : mi_singletonVecKey(rc_src.mi_singletonVecKey) { };
    int getSingletonVecKey() const { return mi_singletonVecKey;};
    void setSingletonKey( int aiKey ) { mi_singletonVecKey = aiKey;};
   protected:
    const ClientBase& operator=( const ClientBase& arc_src )
    { mi_singletonVecKey = arc_src.mi_singletonVecKey; return *this;};
   private:
    int mi_singletonVecKey;
  };
#else
  /** the macro SINGLETON allows to define classes independent from the value
    * of PRT_INSTANCE_CNT, so that the compiler selects the appropriate base class during
    * compile time
    * this variant is used by the compiler, if only one BUS (common case) has to be managed by
    * the IsoAgLib, so that the additional overhead of SingletonVec is avoided
    */
  #define SINGLETON( CLASS ) Singleton<CLASS>
  /** the macro SINGLETON_DERIVED allows to define classes independent from the value
    * of PRT_INSTANCE_CNT, so that the compiler selects the appropriate base class during
    * compile time
    * this variant is used by the compiler, if only one BUS (common case) has to be managed by
    * the IsoAgLib, so that the additional overhead of SingletonVec is avoided
    * Difference to SINGLETON is the fact, that this version allows to derive the singleton pattern using class from
    * another class. This is important to avoid multiple inheritance.
    */
  #define SINGLETON_DERIVED( CLASS, BASE ) SingletonDerived<CLASS,BASE>

  /** the macro SINGLETON_CLIENT1 allows to define a singleton, which allows ONE (aka 1)
    * client class to register pointers to each instance (the register function has to be called
    * during execution of constructor, whereas the unregister function has to be called during
    * execution of destructor. This macro allows to define specific classes independent from
    * the value of PRT_INSTANCE_CNT. With the help of the macro autoInstance(), the interaction of
    * instances of different classes which belongs to the same BUS can be defined independent
    * from PRT_INSTANCE_CNT
    */
  #define SINGLETON_CLIENT1( CLASS, CLIENT1, KEY1 ) SingletonCont1<CLASS,CLIENT1,KEY1>
  /** the macro SINGLETON_CLIENT1 allows to define a singleton, which allows ONE (aka 1)
    * client class to register pointers to each instance (the register function has to be called
    * during execution of constructor, whereas the unregister function has to be called during
    * execution of destructor. This macro allows to define specific classes independent from
    * the value of PRT_INSTANCE_CNT. With the help of the macro autoInstance(), the interaction of
    * instances of different classes which belongs to the same BUS can be defined independent
    * from PRT_INSTANCE_CNT
    */
  #define SINGLETON_DERIVED_CLIENT1( CLASS, BASE, CLIENT1, KEY1 ) SingletonDerivedCont1<CLASS,BASE,CLIENT1,KEY1>

  /** the macro SINGLETON_CLIENT2 allows to define a singleton, which allows TWO (aka 2)
    * client classes to register pointers to each instance (the register function has to be called
    * during execution of constructor, whereas the unregister function has to be called during
    * execution of destructor. This macro allows to define specific classes independent from
    * the value of PRT_INSTANCE_CNT. With the help of the macro autoInstance(), the interaction of
    * instances of different classes which belongs to the same BUS can be defined independent
    * from PRT_INSTANCE_CNT
    */
  #define SINGLETON_CLIENT2( CLASS, CLIENT1, KEY1, CLIENT2, KEY2 ) SingletonCont2<CLASS,CLIENT1,KEY1,CLIENT2,KEY2>
  /** the macro SINGLETON_CLIENT2 allows to define a singleton, which allows TWO (aka 2)
    * client classes to register pointers to each instance (the register function has to be called
    * during execution of constructor, whereas the unregister function has to be called during
    * execution of destructor. This macro allows to define specific classes independent from
    * the value of PRT_INSTANCE_CNT. With the help of the macro autoInstance(), the interaction of
    * instances of different classes which belongs to the same BUS can be defined independent
    * from PRT_INSTANCE_CNT
    */
  #define SINGLETON_DERIVED_CLIENT2( CLASS, BASE, CLIENT1, KEY1, CLIENT2, KEY2 ) \
    SingletonDerivedCont2<CLASS,BASE,CLIENT1,KEY1,CLIENT2,KEY2>

  /** the macro SINGLETON_CLIENT3 allows to define a singleton, which allows THREE (aka 3)
    * client classes to register pointers to each instance (the register function has to be called
    * during execution of constructor, whereas the unregister function has to be called during
    * execution of destructor. This macro allows to define specific classes independent from
    * the value of PRT_INSTANCE_CNT. With the help of the macro autoInstance(), the interaction of
    * instances of different classes which belongs to the same BUS can be defined independent
    * from PRT_INSTANCE_CNT
    */
  #define SINGLETON_DERIVED_CLIENT3( CLASS, CLIENT1, KEY1, CLIENT2, KEY2, CLIENT3, KEY3 ) \
    SingletonCont3< CLASS,CLIENT1,KEY1,CLIENT2,KEY2,CLIENT3,KEY3>
  /** the macro SINGLETON_CLIENT3 allows to define a singleton, which allows THREE (aka 3)
    * client classes to register pointers to each instance (the register function has to be called
    * during execution of constructor, whereas the unregister function has to be called during
    * execution of destructor. This macro allows to define specific classes independent from
    * the value of PRT_INSTANCE_CNT. With the help of the macro autoInstance(), the interaction of
    * instances of different classes which belongs to the same BUS can be defined independent
    * from PRT_INSTANCE_CNT
    */
  #define SINGLETON_CLIENT3( CLASS, BASE, CLIENT1, KEY1, CLIENT2, KEY2, CLIENT3, KEY3 ) \
    SingletonDerivedCont3< CLASS,BASE,CLIENT1,KEY1,CLIENT2,KEY2,CLIENT3,KEY3>

  /** the macro autoInstance() is simply replaced by instance() in cases where only one BUS is managed
    * as the more usual case is only one BUS, this solution creates no unneeded overhead
    */
  #define autoInstance()                  instance()

  #define SINGLETON_VEC_KEY_PARAMETER_DEF
  #define SINGLETON_VEC_KEY_PARAMETER_DEFAULT_NULL_DEF
  #define SINGLETON_VEC_KEY_PARAMETER_DEF_WITH_COMMA
  #define SINGLETON_VEC_KEY_PARAMETER_DEFAULT_NULL_DEF_WITH_COMMA
  #define SINGLETON_VEC_KEY_PARAMETER_USE
  #define SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA

  #define SINGLETON_PAR_DOT_DEF(PAR)
  #define SINGLETON_PAR_ARR_DEF(PAR)
  #define SINGLETON_PAR_BASE_DEF(PAR)

  #define SINGLETON_MC_DATA_ASSIGN
  #define SINGLETON_MEMBER_DEF
  #define SINGLETON_MEMBER_ASSIGN(PAR)
  #define SINGLETON_MEMBER_CONSTRUCTOR
  #define SINGLETON_PARENT_CONSTRUCTOR
  #define SINGLETON_VEC_KEY_INIT_CALL

  #define SINGLETON_VEC_KEY
  #define SINGLETON_VEC_KEY_WITH_COMMA

  #define getCanInstance4Comm()           getCanInstance()
  #define getSchedulerInstance4Comm()     getSchedulerInstance()
  #define getTimePosGpsInstance4Comm()    getTimePosGpsInstance()
  #define getTracGeneralInstance4Comm()   getTracGeneralInstance()
  #define getTracMoveInstance4Comm()      getTracMoveInstance()
  #define getTracLightInstance4Comm()     getTracLightInstance()
  #define getTracAuxInstance4Comm()       getTracAuxInstance( )
  #define getTracPtoInstance4Comm()       getTracPtoInstance()
  #define getIsoMonitorInstance4Comm()    getIsoMonitorInstance()
  #define getProcessInstance4Comm()       getProcessInstance()
  #define getMultiSendInstance4Comm()     getMultiSendInstance()
  #define getIsoTerminalInstance4Comm()   getIsoTerminalInstance()
  #define getMultiReceiveInstance4Comm()  getMultiReceiveInstance()
  #define getIsoRequestPgnInstance4Comm() getIsoRequestPgnInstance()
  #define getMaskHandlerInstance4Comm()   getMaskHandlerInstance()
  #define getIsoFilterManagerInstance4Comm() getIsoFilterManagerInstance()
  #define getMasterGuiInstance4Comm()     getMasterGuiInstance()
  #define getProprietaryMessageHandlerInstance4Comm() getProprietaryMessageHandlerInstance()

  /** the class ClientBase delivers the base information, to concat client class instances
    * with the corresponding server class instance. This is realized by the single
    * attribute singletonVecKey, which is evaluated by the macro autoInstance()
    */
  class ClientBase {
   public:
    /** the constructor accepts an attribute, which is ignored in cases where only one BUS
      * is managed. The variant of ClientBase for more than one BUS uses this parameter
      * to store the dedicated BUS for later accesses by autoInstance()
      */
    ClientBase( int /* ai_singletonVecKey is intentionally unused for one-instance version of Singleton */ = 0 ) {};
    ClientBase( const ClientBase& /* rc_src is intentionally unused for one-instance version of Singleton */ ) { };

    int getSingletonVecKey() const { return 0;};
    void setSingletonKey( int /* riKey is intentionally unused for one-instance version of Singleton */ ) { };
   protected:
    const ClientBase& operator=( const ClientBase& /* arc_src is intentionally unused for one-instance version of Singleton */ ) { return *this;};
  };
#endif
#if defined(CAN_INSTANCE_CNT) && (CAN_INSTANCE_CNT != 1 )
  /** CAN_SINGLETON allows to define CAN classes independent from the specified
    * amount of managed CAN BUSes.
    */
  #define CAN_SINGLETON( CLASS ) SingletonVec<CLASS,CAN_INSTANCE_CNT>
#else
  /** CAN_SINGLETON allows to define CAN classes independent from the specified
    * amount of managed CAN BUSes.
    */
  #define CAN_SINGLETON( CLASS ) Singleton<CLASS>
#endif

#if defined(RS232_INSTANCE_CNT) && (RS232_INSTANCE_CNT != 1 )
  /** RS232_SINGLETON allows to define RS232 classes independent from the specified
    * amount of managed RS232 BUSes.
    */
  #define RS232_SINGLETON( CLASS ) SingletonVec<CLASS,RS232_INSTANCE_CNT>
#else
  /** RS232_SINGLETON allows to define RS232 classes independent from the specified
    * amount of managed RS232 BUSes.
    */
  #define RS232_SINGLETON( CLASS ) Singleton<CLASS>
#endif

/** @todo try to remove these include hacks, as soon as Tasking provides a bugfix
    for their broken version 7.x STL headers.
    Official STL headers like "c166/include.cpp/stl_algobase.h"
    where the C-Header <string.h> is imported into the global namespace, prevent
    a later include of <cstring> ( the correct C++ standard method ) to get
    functions like memmove into "STL_NAMESPACE::" namespace.
    To avoid comparable problems with other compilers, and to avoid any compatibility
    problems with other correct compilers, but which are not used by the maintainers,
    these includes are used for ALL compiler types ( there is only a small impact on
    compile time - but no impact on resulting program ).
*/
#include <new>
#include <cstring>
#include <cstdio>
#include <cstdlib>	// Include before vector or else CNAMESPACE stuff is screwed up for Tasking
#include <vector>

/** BaseSingleton class for classes which need only ONE instance per project
  */
template<class T> class Singleton
{
 public:
  /** return reference to the singleton instance
    * prevent parallel access to initialisation by several tasks
    * @return reference to singleton instance
    */
  static T& instance( void );
  /** avoid error messages, if this function is called with int parameter
    * in cases, where only a single singleton instance is defined by PRT_INSTANCE_CNT
    */
  static T& instance( int riIndex ) { return instance();};
 protected:
  int getSingletonVecKey() const { return 0;};
  static T* mspc_instance;
};
/** BaseSingleton class for classes which need only ONE instance per project.
    This version of Singleton can be derived from another class.
    This is important to avoid multiple inheritance in the class that uses the
    singleton pattern.
  */
template<class T, class B> class SingletonDerived : public B
{
 public:
  /** return reference to the singleton instance
    * prevent parallel access to initialisation by several tasks
    * @return reference to singleton instance
    */
  static T& instance( void );
  /** avoid error messages, if this function is called with int parameter
    * in cases, where only a single singleton instance is defined by PRT_INSTANCE_CNT
    */
  static T& instance( int riIndex ) { return instance();};
 protected:
  int getSingletonVecKey() const { return 0;};
  static T* mspc_instance;
};

// define pointer instance
template<class T> T* Singleton<T>::mspc_instance = (T*)0;
template<class T, class B> T* SingletonDerived<T,B>::mspc_instance = (T*)0;

/** return reference to the singleton instance
  * prevent parallel access to initialisation by several tasks
  * @return reference to singleton instance
  */
template<class T> T& Singleton<T>::instance( void )
{
  if ( mspc_instance > (T*)1 )
  { // is already complete initialized -> this is THE MOST OFTEN CASE
    return *mspc_instance;
  }
  else if ( mspc_instance == (T*)0 )
  { // in case of parallel access from two tasks, the first tasks block further
    // inits with setting the poitner to 1 -> later tasks perform busy waiting
    if ( mspc_instance == (T*)1 )
    { // perform busy waiting till first task is ready with init
      while ( mspc_instance == (T*)1 );
      // as soon as the pointer is != 1 the init is ready
      return *mspc_instance;
    }
    // if execution reaches this point, Singleton::instance() is called first time
    mspc_instance = (T*)1; // block further calls till init is ready
    #ifdef WIN32
    // Microsoft Visual Studio has problems with init of function scope static instances
    mspc_instance = new T;
    #else
    static T sc_instance;
    #endif
    // initialise the instance (in embedded systems, the constructor is NOT called for static var)
    #ifdef WIN32
    mspc_instance->singletonInit();
    #else
    sc_instance.singletonInit();
    // set static pointerto instance
    mspc_instance = &sc_instance;
    #endif
  }
  else
  { // is set to 1 -> make busy wait ( in case this position is reached due to circular init call,
    // we'll get a definitive endless loop here - fine to debug ;-)
    while ( mspc_instance == (T*)1 );
  }
  return *mspc_instance;
};

/** return reference to the singleton instance
  * prevent parallel access to initialisation by several tasks
  * @return reference to singleton instance
  */
template<class T, class B> T& SingletonDerived<T,B>::instance( void )
{
  if ( mspc_instance > (T*)1 )
  { // is already complete initialized -> this is THE MOST OFTEN CASE
    return *mspc_instance;
  }
  else if ( mspc_instance == (T*)0 )
  { // in case of parallel access from two tasks, the first tasks block further
    // inits with setting the poitner to 1 -> later tasks perform busy waiting
    if ( mspc_instance == (T*)1 )
    { // perform busy waiting till first task is ready with init
      while ( mspc_instance == (T*)1 );
      // as soon as the pointer is != 1 the init is ready
      return *mspc_instance;
    }
    // if execution reaches this point, Singleton::instance() is called first time
    mspc_instance = (T*)1; // block further calls till init is ready
    #ifdef WIN32
    // Microsoft Visual Studio has problems with init of function scope static instances
    mspc_instance = new T;
    #else
    static T sc_instance;
    #endif
    // initialise the instance (in embedded systems, the constructor is NOT called for static var)
    #ifdef WIN32
    mspc_instance->singletonInit();
    #else
    sc_instance.singletonInit();
    // set static pointerto instance
    mspc_instance = &sc_instance;
    #endif
  }
  else
  { // is set to 1 -> make busy wait ( in case this position is reached due to circular init call,
    // we'll get a definitive endless loop here - fine to debug ;-)
    while ( mspc_instance == (T*)1 );
  }
  return *mspc_instance;
};


/** scalable variant of base Singleton, which can manage a predefined amount of instances with global access
  */
template<class T, int SIZE> class SingletonVec
{
 public:
  /** return reference to the singleton instance
    * prevent parallel access to initialisation by several tasks
    * @param riIndex selection of the wanted static instance (default first entry)
    * @return reference to singleton instance
    */
  static T& instance( int riIndex = 0 );
 protected:
  int getSingletonVecKey() const { return singletonVecKey;};
  static T * mspc_instance[SIZE];
 private:
  int singletonVecKey;
};

/** scalable variant of base Singleton, which can manage a predefined amount of instances with global access
    This version of Singleton can be derived from another class.
    This is important to avoid multiple inheritance in the class that uses the
    singleton pattern.
  */
template<class T, class B, int SIZE> class SingletonDerivedVec : public B
{
 public:
  /** return reference to the singleton instance
    * prevent parallel access to initialisation by several tasks
    * @param riIndex selection of the wanted static instance (default first entry)
    * @return reference to singleton instance
    */
  static T& instance( int riIndex = 0 );
 protected:
  int getSingletonVecKey() const { return singletonVecKey;};
  static T * mspc_instance[SIZE];
 private:
  int singletonVecKey;
};

// define pointer instance
template<class T, int SIZE> T * SingletonVec<T,SIZE>::mspc_instance[SIZE] = {(T*)0};
template<class T, class B, int SIZE> T * SingletonDerivedVec<T,B,SIZE>::mspc_instance[SIZE] = {(T*)0};

/** return reference to the singleton instance
  * prevent parallel access to initialisation by several tasks
  * @param riIndex selection of the wanted static instance (default first entry)
  * @return reference to singleton instance
  */
template<class T, int SIZE> T & SingletonVec<T,SIZE>::instance( int riIndex )
{
  if ( mspc_instance[0] > (T*)1 )
  { // is already complete initialized -> this is THE MOST OFTEN CASE
    return *mspc_instance[riIndex];
  }
  else if ( mspc_instance[0] == (T*)0 )
  { // in case of parallel access from two tasks, the first tasks block further
    // inits with setting the poitner to 1 -> later tasks perform busy waiting
    if ( mspc_instance[0] == (T*)1 )
    { // perform busy waiting till first task is ready with init
      while ( mspc_instance[0] == (T*)1 );
      // as soon as the pointer is != 1 the init is ready
      return *mspc_instance[riIndex];
    }
    // if execution reaches this point, Singleton::instance() is called first time
    mspc_instance[0] = (T*)1; // block further calls till init is ready
    #ifdef WIN32
    for ( int i = 0; i < SIZE; i++)
    {
      mspc_instance[i] = new T;
      mspc_instance[i]->singletonVecKey = i;
      mspc_instance[i]->singletonInit();
    }
    #else
    static T sc_instance[SIZE];
    for ( int i = 0; i < SIZE; i++)
    { // initialise the instance (in embedded systems, the constructor is NOT called for static var)
      sc_instance[i].singletonVecKey = i;
      sc_instance[i].singletonInit();
      // set static pointerto instance
      mspc_instance[i] = &sc_instance[i];
    }
    #endif
  }
  else
  { // is set to 1 -> make busy wait ( in case this position is reached due to circular init call,
    // we'll get a definitive endless loop here - fine to debug ;-)
    while ( mspc_instance[0] == (T*)1 );
  }
  return *mspc_instance[riIndex];
};
/** return reference to the singleton instance
  * prevent parallel access to initialisation by several tasks
  * @param riIndex selection of the wanted static instance (default first entry)
  * @return reference to singleton instance
  */
template<class T, class B, int SIZE> T & SingletonDerivedVec<T,B,SIZE>::instance( int riIndex )
{
  if ( mspc_instance[0] > (T*)1 )
  { // is already complete initialized -> this is THE MOST OFTEN CASE
    return *mspc_instance[riIndex];
  }
  else if ( mspc_instance[0] == (T*)0 )
  { // in case of parallel access from two tasks, the first tasks block further
    // inits with setting the poitner to 1 -> later tasks perform busy waiting
    if ( mspc_instance[0] == (T*)1 )
    { // perform busy waiting till first task is ready with init
      while ( mspc_instance[0] == (T*)1 );
      // as soon as the pointer is != 1 the init is ready
      return *mspc_instance[riIndex];
    }
    // if execution reaches this point, Singleton::instance() is called first time
    mspc_instance[0] = (T*)1; // block further calls till init is ready
    #ifdef WIN32
    for ( int i = 0; i < SIZE; i++)
    {
      mspc_instance[i] = new T;
      mspc_instance[i]->singletonVecKey = i;
      mspc_instance[i]->singletonInit();
    }
    #else
    static T sc_instance[SIZE];
    for ( int i = 0; i < SIZE; i++)
    { // initialise the instance (in embedded systems, the constructor is NOT called for static var)
      sc_instance[i].singletonVecKey = i;
      sc_instance[i].singletonInit();
      // set static pointerto instance
      mspc_instance[i] = &sc_instance[i];
    }
    #endif
  }
  else
  { // is set to 1 -> make busy wait ( in case this position is reached due to circular init call,
    // we'll get a definitive endless loop here - fine to debug ;-)
    while ( mspc_instance[0] == (T*)1 );
  }
  return *mspc_instance[riIndex];
};

/* ***************** define several variants of template for Singleton base class, which *******************
 * ***************** allows up to three different client classes to register             *******************
 * ***************** use macros, to get SAME implementation for different variants       *******************
 * ***************** as the specific functions for the client types must have different  *******************
 * ***************** names, this can't be handled without macros                         *******************
 */
/** first define bunch of Singleton templates which are NOT derived from another class */
#define SINGLETON_TYPE Singleton
#define TEMPLATE_DECL_BASE class T
#define TEMPLATE_QUAL_BASE T
/* first define the container singleton templates which are based on the unique instance Singleton */
#define USE_BASE_SINGLETON_VEC 0
/* first define the container for only one client type */
#define CLIENT_TYPE_CNT() 1
/* by multiple inclusion of singleton_container.h the suitable template class can be generated */
#include "singleton_container.h"

/* first define the container for two client types */
#undef CLIENT_TYPE_CNT
#define CLIENT_TYPE_CNT() 2
/* by multiple inclusion of singleton_container.h the suitable template class can be generated */
#include "singleton_container.h"

/* first define the container for three client types */
#undef CLIENT_TYPE_CNT
#define CLIENT_TYPE_CNT() 3
/* by multiple inclusion of singleton_container.h the suitable template class can be generated */
#include "singleton_container.h"

/* define the container singleton templates which are based on the unique instance Singleton */
#undef USE_BASE_SINGLETON_VEC
#define USE_BASE_SINGLETON_VEC 1
/* first define the container for only one client type */
#undef CLIENT_TYPE_CNT
#define CLIENT_TYPE_CNT() 1
/* by multiple inclusion of singleton_container.h the suitable template class can be generated */
#include "singleton_container.h"

/* first define the container for two client types */
#undef CLIENT_TYPE_CNT
#define CLIENT_TYPE_CNT() 2
/* by multiple inclusion of singleton_container.h the suitable template class can be generated */
#include "singleton_container.h"

/* first define the container for three client types */
#undef CLIENT_TYPE_CNT
#define CLIENT_TYPE_CNT() 3
/* by multiple inclusion of singleton_container.h the suitable template class can be generated */
#include "singleton_container.h"

#undef SINGLETON_TYPE
#undef TEMPLATE_DECL_BASE
#undef TEMPLATE_QUAL_BASE
/**
  Second bunch of Singleton Templates which can be derived from another class, so that
  multiple inheritance can be avoided
*/
#define SINGLETON_TYPE SingletonDerived
#define TEMPLATE_DECL_BASE class T, class B
#define TEMPLATE_QUAL_BASE T,B

/* first define the container singleton templates which are based on the unique instance Singleton */
#undef USE_BASE_SINGLETON_VEC
#define USE_BASE_SINGLETON_VEC 0
/* first define the container for only one client type */
#undef CLIENT_TYPE_CNT
#define CLIENT_TYPE_CNT() 1
/* by multiple inclusion of singleton_container.h the suitable template class can be generated */
#include "singleton_container.h"

/* first define the container for two client types */
#undef CLIENT_TYPE_CNT
#define CLIENT_TYPE_CNT() 2
/* by multiple inclusion of singleton_container.h the suitable template class can be generated */
#include "singleton_container.h"

/* first define the container for three client types */
#undef CLIENT_TYPE_CNT
#define CLIENT_TYPE_CNT() 3
/* by multiple inclusion of singleton_container.h the suitable template class can be generated */
#include "singleton_container.h"

/* define the container singleton templates which are based on the unique instance Singleton */
#undef USE_BASE_SINGLETON_VEC
#define USE_BASE_SINGLETON_VEC 1
/* first define the container for only one client type */
#undef CLIENT_TYPE_CNT
#define CLIENT_TYPE_CNT() 1
/* by multiple inclusion of singleton_container.h the suitable template class can be generated */
#include "singleton_container.h"

/* first define the container for two client types */
#undef CLIENT_TYPE_CNT
#define CLIENT_TYPE_CNT() 2
/* by multiple inclusion of singleton_container.h the suitable template class can be generated */
#include "singleton_container.h"

/* first define the container for three client types */
#undef CLIENT_TYPE_CNT
#define CLIENT_TYPE_CNT() 3
/* by multiple inclusion of singleton_container.h the suitable template class can be generated */
#include "singleton_container.h"

/// @todo REMOVE! This is just to be compatible with older versionen vt2iso generated files.
/// This can be removed after some migration time. State now is 20 Sep 2007.
#define SINGLETON_VEC_KEY_PARAMETER_VAR_WITH_COMMA SINGLETON_VEC_KEY_PARAMETER_USE_WITH_COMMA

#endif
