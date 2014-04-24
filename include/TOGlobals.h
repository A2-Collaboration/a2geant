// SVN Info: $Id: TOGlobals.h 486 2009-08-19 15:11:46Z werthm $

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


#ifndef P2MKIN_TOGlobals
#define P2MKIN_TOGlobals

#include "TDatabasePDG.h"


namespace TOGlobals
{
    extern TDatabasePDG* fPDG_DB;

    static const Double_t kDeuteronMass  = 1875.612762;
    static const Double_t kProtonMass    = 938.27203;
    static const Double_t kNeutronMass   = 939.56536;
    static const Double_t kEtaMass       = 547.853;
    static const Double_t kPi0Mass       = 134.9766;
    static const Double_t kPiChargedMass = 139.57018;
    static const Double_t kHe3Mass       = 2808.391;
    static const Double_t kDiprotonMass  = 1871.4;
    
    static const Double_t kHbarGeVfm     = 0.197463569747999998;     // unit: GeV*fm/c
    
    TDatabasePDG* GetPDG_DB();
    static const Int_t kDeuteronID       = 99999;
}

#endif

