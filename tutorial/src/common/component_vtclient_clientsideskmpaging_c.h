#ifndef COMPONENT_VTCLIENT_CLIENTSIDESKMPAGING_C_H
#define COMPONENT_VTCLIENT_CLIENTSIDESKMPAGING_C_H

#include <IsoAgLib/comm/Part6_VirtualTerminal_Client/ivtclientservercommunication_c.h>


namespace IsoAgLibTutorial
{
  //! Helper class for ClientSide SKM Paging
  //! This handling is needed if the objectpool
  //! has SKMs with more than 6 SKs.
  //! A VT does not have to support some sort of
  //! paging-mechanism. For example the John Deere
  //! GS2 has only 10 virtual SKs...
  class ClientSideSkmPaging_c
  {
  public:
    ClientSideSkmPaging_c()
      : mi_page (0) // needs to be initialized via "init", when used and information known
      , mi_numPages (0) // needs to be initialized via "init", when used and information known
      , mi_numKeysPerPage (0) // needs to be initialized via "init", when information known
    {}

    //! The implementation of this virtual init function has to calculate the
    //! member variables mi_page, mi_numPages and mi_numKeysPerPage based on
    //! the virtual and physical number of softkeys as provided by arc_vtCSC's
    //! iVtServerInstance_c.
    virtual void init (IsoAgLib::iVtClientServerCommunication_c& arc_vtCSC) = 0;

    //! This function switches to the next page.
    //! Typically call this function when your custom
    //! "Paging" softkey press is being detected.
    void switchPage();

  protected:
    int getPage() { return mi_page; }
    int getNumPages() { return mi_numPages; }
    int getNumKeysPerPage() { return mi_numKeysPerPage; }

  private:
    //! The implementation of the virtual function needs to connect
    //! the current softkeymask-page to all required data-/alarmmasks
    //! depending on "mi_numKeysPerPage" and "mi_page".
    virtual void setPage() = 0;

  protected:
    int mi_page;
    int mi_numPages;
    int mi_numKeysPerPage;
  };

} /* namespace IsoAgLibTutorial */


inline
void
IsoAgLibTutorial::ClientSideSkmPaging_c::switchPage()
{
  // if mi_numKeysOnPage==0, then we're not doing our own paging
  if (mi_numPages <= 0)
    return; // this case shouldn't happen - only with bad packets on the bus...

  mi_page += 1;
  mi_page %= mi_numPages;

  setPage();
}


#endif
