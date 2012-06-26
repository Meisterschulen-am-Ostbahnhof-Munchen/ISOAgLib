/*
  container.h: C-style Macros for added container(s) to classes.

  (C) Copyright 2009 - 2012 by OSB AG and developing partners

  See the repository-log for details on the authors and file-history.
  (Repository information can be found at <http://isoaglib.com/download>)

  Use, modification and distribution are subject to the GNU General
  Public License with exceptions for ISOAgLib. (See accompanying
  file LICENSE.txt or copy at <http://isoaglib.com/download/license>)
*/
#ifndef CONTAINER_H
#define CONTAINER_H

#include <vector>

#define CONTAINER_CLIENTx_CTOR_INITIALIZER_LIST(pc_searchCacheCx,c_arrClientCx) \
  c_arrClientCx(), \
  pc_searchCacheCx (c_arrClientCx.begin())

#define CONTAINER_CLIENT1_CTOR_INITIALIZER_LIST CONTAINER_CLIENTx_CTOR_INITIALIZER_LIST (pc_searchCacheC1,c_arrClientC1)
#define CONTAINER_CLIENT2_CTOR_INITIALIZER_LIST CONTAINER_CLIENTx_CTOR_INITIALIZER_LIST (pc_searchCacheC2,c_arrClientC2)
#define CONTAINER_CLIENT3_CTOR_INITIALIZER_LIST CONTAINER_CLIENTx_CTOR_INITIALIZER_LIST (pc_searchCacheC3,c_arrClientC3)

#ifdef OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED
  #define CONTAINER_CLIENTx_MEMBERS(Cx, c_arrClientCx, pc_searchCacheCx, cacheTypeCx_t) \
  private: \
    typedef STL_NAMESPACE::vector<Cx*,MALLOC_TEMPLATE(Cx*)>::iterator cacheTypeCx_t; \
    STL_NAMESPACE::vector<Cx*,MALLOC_TEMPLATE(Cx*)> c_arrClientCx; \
    cacheTypeCx_t pc_searchCacheCx
#else
  #define CONTAINER_CLIENTx_MEMBERS(Cx, c_arrClientCx, pc_searchCacheCx, cacheTypeCx_t) \
  private: \
    typedef STL_NAMESPACE::vector<Cx*>::iterator cacheTypeCx_t; \
    STL_NAMESPACE::vector<Cx*> c_arrClientCx; \
    cacheTypeCx_t pc_searchCacheCx
#endif


#define CONTAINER_CLIENTx_MEMBER_FUNCTIONS_MAIN(registerCx,unregisterCx,Cx,pc_searchCacheCx,c_arrClientCx,cacheTypeCx_t) \
public: \
  bool registerCx( Cx* pc_client ) \
  { \
    bool b_exists = false; \
    for ( pc_searchCacheCx = c_arrClientCx.begin(); \
          pc_searchCacheCx != c_arrClientCx.end(); \
          ++pc_searchCacheCx) \
      if ( *pc_searchCacheCx == pc_client ) { b_exists = true; break;} \
    if ( !b_exists ) \
    { \
      c_arrClientCx.push_back( pc_client ); \
      pc_searchCacheCx = c_arrClientCx.end() - 1; \
    } \
    return true; /* right now we can't detect a failed push_back */ \
  } \
\
  void unregisterCx( Cx* pc_client ) \
  { \
    for ( pc_searchCacheCx = c_arrClientCx.begin(); \
          pc_searchCacheCx != c_arrClientCx.end(); \
          ++pc_searchCacheCx) \
    { \
      if ( *pc_searchCacheCx == pc_client ) \
      { \
        c_arrClientCx.erase( pc_searchCacheCx ); \
        pc_searchCacheCx = c_arrClientCx.begin(); \
        break; \
      } \
    } \
  } \
\
CONTAINER_CLIENTx_MEMBERS(Cx,c_arrClientCx,pc_searchCacheCx,cacheTypeCx_t)


#define CONTAINER_CLIENT1_MEMBER_FUNCTIONS_MAIN(C1) CONTAINER_CLIENTx_MEMBER_FUNCTIONS_MAIN(registerC1,unregisterC1,C1,pc_searchCacheC1,c_arrClientC1,cacheTypeC1_t)
#define CONTAINER_CLIENT2_MEMBER_FUNCTIONS_MAIN(C2) CONTAINER_CLIENTx_MEMBER_FUNCTIONS_MAIN(registerC2,unregisterC2,C2,pc_searchCacheC2,c_arrClientC2,cacheTypeC2_t)
#define CONTAINER_CLIENT3_MEMBER_FUNCTIONS_MAIN(C3) CONTAINER_CLIENTx_MEMBER_FUNCTIONS_MAIN(registerC3,unregisterC3,C3,pc_searchCacheC3,c_arrClientC3,cacheTypeC3_t)

#define CONTAINER_CLIENTx_MEMBER_FUNCTIONS_KEY(existCx,getCx,Cx,Kx,pc_searchCacheCx,c_arrClientCx,cacheTypeCx_t) \
  bool existCx( Kx at_key ) \
  { \
    if ( ( pc_searchCacheCx != c_arrClientCx.end() ) && ( (*pc_searchCacheCx)->operator==( at_key ) ) ) \
      return true; \
\
    for ( pc_searchCacheCx = c_arrClientCx.begin(); \
          pc_searchCacheCx != c_arrClientCx.end(); \
          ++pc_searchCacheCx) \
      if ( (*pc_searchCacheCx)->operator==( at_key ) ) return true; \
\
    pc_searchCacheCx = c_arrClientCx.begin(); \
    return false; \
  } \
\
  Cx& getCx ( Kx at_key ) \
  { \
    const bool __b = existCx( at_key ); \
    isoaglib_assert( __b ); (void)__b; \
    return **pc_searchCacheCx; \
  }


#define CONTAINER_CLIENT1_MEMBER_FUNCTIONS_KEY(C1,K1) CONTAINER_CLIENTx_MEMBER_FUNCTIONS_KEY(existC1,getC1,C1,K1,pc_searchCacheC1,c_arrClientC1,cacheTypeC1_t)
#define CONTAINER_CLIENT2_MEMBER_FUNCTIONS_KEY(C2,K2) CONTAINER_CLIENTx_MEMBER_FUNCTIONS_KEY(existC2,getC2,C2,K2,pc_searchCacheC2,c_arrClientC2,cacheTypeC2_t)
#define CONTAINER_CLIENT3_MEMBER_FUNCTIONS_KEY(C3,K3) CONTAINER_CLIENTx_MEMBER_FUNCTIONS_KEY(existC3,getC3,C3,K3,pc_searchCacheC3,c_arrClientC3,cacheTypeC3_t)

#endif // CONTAINER_H
