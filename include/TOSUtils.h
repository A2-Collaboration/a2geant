// SVN Info: $Id: TOSUtils.h 454 2009-06-21 19:24:30Z werthm $

/*************************************************************************
 * Author: Dominik Werthmueller, 2008
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TOSUtils                                                             //
//                                                                      //
// This namespace contains some often used functions and operations     //
// for strings.                                                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef P2MKIN_TOSUtils
#define P2MKIN_TOSUtils

#ifndef ROOT_Rtypes
#include "Rtypes.h"
#endif

#include <cstring>


namespace TOSUtils
{
    extern Char_t* fTmpCh;
    const Int_t fTmpChLength = 1024;

    Int_t IndexOf(const Char_t* s, Char_t c, UInt_t p = 0);
    Int_t IndexOf(const Char_t* s1, const Char_t* s2, UInt_t p = 0);
    Int_t LastIndexOf(const Char_t* s, Char_t c);
    
    Char_t* ExtractFileName(const Char_t* s);
    Char_t* ExtractPureFileName(const Char_t* s);
    
    Char_t* Trim(const Char_t* s);
    
    Bool_t IsComment(const Char_t* s);

    Char_t* FormatArrayList(Int_t n, Int_t* arr);
    Char_t* FormatArrayList(Int_t n, UInt_t* arr);
    Char_t* FormatArrayList(Int_t n, Double_t* arr);
}

#endif

