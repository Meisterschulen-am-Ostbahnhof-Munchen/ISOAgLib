#ifdef CLASS_NAME
  #undef CLASS_NAME
#endif
#ifdef BASE_CLASS_NAME
  #undef BASE_CLASS_NAME
#endif
#ifdef TEMPLATE_DECL
  #undef TEMPLATE_DECL
#endif
#ifdef TEMPLATE_QUAL
  #undef TEMPLATE_QUAL
#endif

#ifndef gluetoks
  #define gluetoks(a, b)  a ## b
  #define gluetoks3(a, b, c)  a ## b ## c
  #define gluetoks4(a, b, c, d)  a ## b ## c ## d
  #define gluetoks5(a, b, c, d, e)  a ## b ## c ## d ## e
#endif
#ifndef gluemacs
  #define gluemacs(a, b)  gluetoks(a, b)
  #define gluemacs3(a, b, c)  gluetoks3(a, b, c)
  #define gluemacs4(a, b, c, d)  gluetoks4(a, b, c, d)
  #define gluemacs5(a, b, c, d, e)  gluetoks5(a, b, c, d, e)
#endif

#if USE_BASE_SINGLETON_VEC
  #define CLASS_NAME() gluemacs3( SINGLETON_TYPE, VecCont , CLIENT_TYPE_CNT() )
	#define BASE_CLASS_NAME gluemacs( SINGLETON_TYPE, Vec ) < TEMPLATE_QUAL_BASE ,SIZE>
#else
  #define CLASS_NAME() gluemacs3( SINGLETON_TYPE, Cont, CLIENT_TYPE_CNT() )
  #define BASE_CLASS_NAME SINGLETON_TYPE < TEMPLATE_QUAL_BASE >
#endif

#if ( CLIENT_TYPE_CNT() == 3 ) && USE_BASE_SINGLETON_VEC
  #define TEMPLATE_DECL TEMPLATE_DECL_BASE, int SIZE, class C1, typename K1, class C2, typename K2, class C3, typename K3
  #define TEMPLATE_QUAL TEMPLATE_QUAL_BASE,SIZE,C1,K1,C2,K2,C3,K3
#endif
#if ( CLIENT_TYPE_CNT() == 3 ) && (!USE_BASE_SINGLETON_VEC)
    #define TEMPLATE_DECL TEMPLATE_DECL_BASE, class C1, typename K1, class C2, typename K2, class C3, typename K3
    #define TEMPLATE_QUAL TEMPLATE_QUAL_BASE,C1,K1,C2,K2,C3,K3
#endif

#if ( CLIENT_TYPE_CNT() == 2 ) && USE_BASE_SINGLETON_VEC
    #define TEMPLATE_DECL TEMPLATE_DECL_BASE, int SIZE, class C1, typename K1, class C2, typename K2
    #define TEMPLATE_QUAL TEMPLATE_QUAL_BASE,SIZE,C1,K1,C2,K2
#endif
#if ( CLIENT_TYPE_CNT() == 2 ) && (!USE_BASE_SINGLETON_VEC)
    #define TEMPLATE_DECL TEMPLATE_DECL_BASE, class C1, typename K1, class C2, typename K2
    #define TEMPLATE_QUAL TEMPLATE_QUAL_BASE,C1,K1,C2,K2
#endif

#if ( CLIENT_TYPE_CNT() == 1 ) && USE_BASE_SINGLETON_VEC
    #define TEMPLATE_DECL TEMPLATE_DECL_BASE, int SIZE, class C1, typename K1
    #define TEMPLATE_QUAL TEMPLATE_QUAL_BASE,SIZE,C1,K1
#endif
#if ( CLIENT_TYPE_CNT() == 1 ) && (!USE_BASE_SINGLETON_VEC)
    #define TEMPLATE_DECL TEMPLATE_DECL_BASE, class C1, typename K1
    #define TEMPLATE_QUAL TEMPLATE_QUAL_BASE,C1,K1
#endif

/** template class which enables instances of specified template client type
  * to register themselves on construction, so that they can be accessed through
  * this class
  */
template<TEMPLATE_DECL> class CLASS_NAME() : public BASE_CLASS_NAME
{
 protected:
  /** default constructor which ints the search cache */
  CLASS_NAME()( void );
  ~CLASS_NAME()( void );
#define GET_CL_DECL() C1
  #define GET_K_DECL() K1
  #include "singleton_container_decl.h"
  #undef GET_CL_DECL
  #undef GET_K_DECL
#if ( CLIENT_TYPE_CNT() > 1 )
  #define GET_CL_DECL() C2
  #define GET_K_DECL() K2
  #include "singleton_container_decl.h"
  #undef GET_CL_DECL
  #undef GET_K_DECL
#endif
#if ( CLIENT_TYPE_CNT() > 2 )
  #define GET_CL_DECL() C3
  #define GET_K_DECL() K3
  #include "singleton_container_decl.h"
  #undef GET_CL_DECL
  #undef GET_K_DECL
#endif
};

/** default constructor which ints the search cache */
template<TEMPLATE_DECL> CLASS_NAME()<TEMPLATE_QUAL>::CLASS_NAME()( void )
{
  pc_searchCacheC1 = c_arrClientC1.begin();
#if ( CLIENT_TYPE_CNT() > 1 )
  pc_searchCacheC2 = c_arrClientC2.begin();
#endif
#if ( CLIENT_TYPE_CNT() > 2 )
  pc_searchCacheC3 = c_arrClientC3.begin();
#endif
}

/** destructor which ints the search cache */
template<TEMPLATE_DECL> CLASS_NAME()<TEMPLATE_QUAL>::~CLASS_NAME()( void )
{
}

  #define GET_CL_DEF() C1
  #define GET_K_DEF() K1
  #include "singleton_container_decl.h"
  #undef GET_CL_DEF
  #undef GET_K_DEF
#if ( CLIENT_TYPE_CNT() > 1 )
  #define GET_CL_DEF() C2
  #define GET_K_DEF() K2
  #include "singleton_container_decl.h"
  #undef GET_CL_DEF
  #undef GET_K_DEF
#endif
#if ( CLIENT_TYPE_CNT() > 2 )
  #define GET_CL_DEF() C3
  #define GET_K_DEF() K3
  #include "singleton_container_decl.h"
  #undef GET_CL_DEF
  #undef GET_K_DEF
#endif
