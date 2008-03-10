//FileServerClient includes
#include <IsoAgLib/comm/Part13_FileServer_Client/ifsclientservercommunication_c.h>
#include <IsoAgLib/comm/Part13_FileServer_Client/ifsmanager_c.h>
#include <IsoAgLib/comm/Part13_FileServer_Client/ifsclient_c.h>

#include <myfsclient_c.h>

void MyFsClient_c::connect (iIdentItem_c& c_myIdent)
{
    /*
     * apparently, the vector is passed as a whole, but it (apparently)
     * only contains pointers.  Thus, this member ceases to exist from
     * the vector when this method exits.
     */
    static iFsWhitelist_c tmpwlst;
    static std::vector<iFsWhitelist_c *> v_fsWhitelist;
    static std::vector<struct iFsDirectory *> v_volumes;

    #if defined(DEBUG) && defined(SYSTEM_PC)
    fprintf (stderr, "MyFsClient_c::connect: entered\n");
	#endif

    tmpwlst.b_requestRemoveableMedium = true;
    tmpwlst.i32_requestedManufacturer = -1;
    tmpwlst.ui8_minSimultaniouslyOpenFiles = 5;

    v_fsWhitelist.push_back(&tmpwlst);

    // Call to fsManager to init and register client
    mp_fscom = getIFsManagerInstance().initFsClient(c_myIdent, *this, v_fsWhitelist);
    if (mp_fscom == NULL) {
#if defined(DEBUG) && defined(SYSTEM_PC)
	fprintf (stderr, "Could not init fsClient\n");
	abort();
#else
	return;
#endif
    }
}


char *MyFsClient_c::confvar (const char *varname)
{
    if (conftbl != NULL)
    	return conftbl;	/* currently the only thing there */

    return NULL;
}


void MyFsClient_c::confvar (const char *varname, char *value)
{
    int len = strlen (value);

    if (strlen (conftbl) >= len)
    	strcpy (conftbl, value);	/* ignore shrink */
    else {
	char *tbl;

    	tbl = (char *)realloc (conftbl, len);
	if (tbl == NULL)
	    fprintf (stderr, "confvar: no space: variable lost\n");
	else {
	    conftbl = tbl;
	    strcpy (conftbl, value);
	}
    }
}


void MyFsClient_c::load_conftbl ()
{
    /*
     * The initial profile read has completed.  Store it in the
     * configuration tbl.
     */
    if (conftbl != NULL)
        free (conftbl);

    conftbl = strdup ((const char *)received_data);
}




