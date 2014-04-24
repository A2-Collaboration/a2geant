// SVN Info: $Id: TOGlobals.cxx 486 2009-08-19 15:11:46Z werthm $

/*************************************************************************
 * Author: Dominik Werthmueller, 2008
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TOGlobals                                                            //
//                                                                      //
// This namespace grants access to global objects and methods.          //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "TOGlobals.h"

#if !defined(R__ALPHA) && !defined(R__SOLARIS) && !defined(R__ACC) && !defined(R__FBSD)
NamespaceImp(TOGlobals)
#endif


// Declare namespace member
TDatabasePDG* TOGlobals::fPDG_DB = 0;


//______________________________________________________________________________
TDatabasePDG* TOGlobals::GetPDG_DB()
{
    // Returns a pointer to the global PDG database object.

    // Create database if necessary
    if (!fPDG_DB) 
    {
        fPDG_DB = new TDatabasePDG;
        fPDG_DB->ReadPDGTable();
    
        // add the deuteron to the database
        fPDG_DB->AddParticle("deuteron", "deuteron", kDeuteronMass/1000., kTRUE,
                             0., 3., "deuteron", kDeuteronID, 0, 45);
    }

    return fPDG_DB;
}

