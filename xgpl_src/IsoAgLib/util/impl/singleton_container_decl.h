  #if defined( GET_CL_DECL ) && defined( GET_K_DECL )
  /** register pointer to a new client
    * this function is called within construction of new client instance
    */
  bool gluemacs( register, GET_CL_DECL() )( GET_CL_DECL()* pc_client );
  /** unregister a client instance, which is destructed at the moment
    * this function is called within the destructor of the client instance
    */
  void gluemacs( unregister, GET_CL_DECL() )( GET_CL_DECL()* pc_client );
  /** check if an instance is registered with the given Key
    * the instantiating class must have a operator==( K )
    * @param rt_key key value to identify the searched entry
    * @return true -> searched item exists
    */
  bool gluemacs( exist, GET_CL_DECL() )( GET_K_DECL() rt_key );
  /** deliver a reference to a stored client, for which
    * operator==( rt_key ) result TRUE
    * IMPORTANT: the instantiating class MUST have a function
    * registerAccessFlt( void ), where an individual reaction on
    * undefined access is implemented
    * @param rt_key key of the searched client instance
    * @return instance of the searched client instance
    */
  GET_CL_DECL()& gluemacs( get, GET_CL_DECL() ) ( GET_K_DECL() rt_key );

  /** vector with pointers to registered client instances */
	#ifdef OPTIMIZE_HEAPSIZE_IN_FAVOR_OF_SPEED
  STL_NAMESPACE::vector<GET_CL_DECL()*,STL_NAMESPACE::__malloc_alloc_template<0> > gluemacs( c_arrClient, GET_CL_DECL() );
  typedef typename STL_NAMESPACE::vector<GET_CL_DECL()*,STL_NAMESPACE::__malloc_alloc_template<0> >::iterator gluemacs( gluemacs( cacheType, GET_CL_DECL() ), _t);
	#else
  STL_NAMESPACE::vector<GET_CL_DECL()*> gluemacs( c_arrClient, GET_CL_DECL() );
  typedef typename STL_NAMESPACE::vector<GET_CL_DECL()*>::iterator gluemacs( gluemacs( cacheType, GET_CL_DECL() ), _t);
	#endif

  gluemacs( gluemacs( cacheType, GET_CL_DECL() ), _t) gluemacs( pc_searchCache, GET_CL_DECL() );
#endif
#if defined( GET_CL_DEF ) && defined( GET_K_DEF )
  /** register pointer to a new client
    * this function is called within construction of new client instance
    */
template<TEMPLATE_DECL> bool CLASS_NAME()<TEMPLATE_QUAL>::gluemacs( register, GET_CL_DEF() )( GET_CL_DEF()* pc_client )
{ // find search the array for another entry with the same pointer value
  bool b_exists = false;
  bool b_result = false;
  for ( gluemacs( pc_searchCache, GET_CL_DEF() ) = gluemacs( c_arrClient, GET_CL_DEF() ).begin();
        gluemacs( pc_searchCache, GET_CL_DEF() ) != gluemacs( c_arrClient, GET_CL_DEF() ).end();
        gluemacs( pc_searchCache, GET_CL_DEF() )++)
    if ( *gluemacs( pc_searchCache, GET_CL_DEF() ) == pc_client ) { b_exists = true; break;}
  if ( !b_exists )
  { // given entry is new -> insert in list
    size_t oldSize = gluemacs( c_arrClient, GET_CL_DEF() ).size();
    gluemacs( c_arrClient, GET_CL_DEF() ).push_back( pc_client );
    // test if vector grew by one entry
    if ( gluemacs( c_arrClient, GET_CL_DEF() ).size() == ( oldSize + 1 ) ) b_result = true;
    gluemacs( pc_searchCache, GET_CL_DEF() ) = gluemacs( c_arrClient, GET_CL_DEF() ).end() - 1;
  }
  return b_result;
}

  /** unregister a client instance, which is destructed at the moment
    * this function is called within the destructor of the client instance
    */
template<TEMPLATE_DECL> void CLASS_NAME()<TEMPLATE_QUAL>::gluemacs( unregister, GET_CL_DEF() )( GET_CL_DEF()* pc_client )
{ // find search the array for the entry which shall be deleted
  for ( gluemacs( pc_searchCache, GET_CL_DEF() ) = gluemacs( c_arrClient, GET_CL_DEF() ).begin();
        gluemacs( pc_searchCache, GET_CL_DEF() ) != gluemacs( c_arrClient, GET_CL_DEF() ).end();
        gluemacs( pc_searchCache, GET_CL_DEF() )++)
  {
    if ( *gluemacs( pc_searchCache, GET_CL_DEF() ) == pc_client )
    { // item which shall be erased from list is found
      gluemacs( c_arrClient, GET_CL_DEF() ).erase( gluemacs( pc_searchCache, GET_CL_DEF() ) );
      gluemacs( pc_searchCache, GET_CL_DEF() ) = gluemacs( c_arrClient, GET_CL_DEF() ).begin();
      break;
    }
  }
}

/** check if an instance is registered with the given Key
  * the instantiating class must have a operator==( K )
  * @param rt_key key value to identify the searched entry
  * @return true -> searched item exists
  */
template<TEMPLATE_DECL> bool CLASS_NAME()<TEMPLATE_QUAL>::gluemacs( exist, GET_CL_DEF() )( GET_K_DEF() rt_key )
{ // first check if the actual search cache iterator points to the searched item
  if ( ( gluemacs( pc_searchCache, GET_CL_DEF() ) != gluemacs( c_arrClient, GET_CL_DEF() ).end() ) && ( (*gluemacs( pc_searchCache, GET_CL_DEF() ))->operator==( rt_key ) ) )
    return true;
  // now search whole container
  for ( gluemacs( pc_searchCache, GET_CL_DEF() ) = gluemacs( c_arrClient, GET_CL_DEF() ).begin();
        gluemacs( pc_searchCache, GET_CL_DEF() ) != gluemacs( c_arrClient, GET_CL_DEF() ).end();
        gluemacs( pc_searchCache, GET_CL_DEF() )++)
    if ( (*gluemacs( pc_searchCache, GET_CL_DEF() ))->operator==( rt_key ) ) return true;
  // if execution reaches this point, the searched element is not found
  // set cache to array begin
  gluemacs( pc_searchCache, GET_CL_DEF() ) = gluemacs( c_arrClient, GET_CL_DEF() ).begin();
  return false;
}
/** deliver a reference to a stored client, for which
  * operator==( rt_key ) result TRUE
  * IMPORTANT: the instantiating class MUST have a function
  * registerAccessFlt( void ), where an individual reaction on
  * undefined access is implemented
  * @param rt_key key of the searched client instance
  * @return instance of the searched client instance
  */
template<TEMPLATE_DECL> GET_CL_DEF()& CLASS_NAME()<TEMPLATE_QUAL>::gluemacs( get, GET_CL_DEF() )( GET_K_DEF() rt_key )
{ // use gluemacs( exist, GET_CL_DEF() ) function to search for element -> this function sets cache
  // to searched element on success
  if ( !gluemacs( exist, GET_CL_DEF() )( rt_key ) )
  { // if execution reaches this point, the searched element is NOT found
    CLASS_NAME()<TEMPLATE_QUAL>::autoInstance().registerAccessFlt();
    // gluemacs( exist, GET_CL_DEF() ) sets cache to begin of array if searched item not found -> return it
  }
  // cache points now to searched item OR to default beginning
  return **gluemacs( pc_searchCache, GET_CL_DEF() );
}

#endif
