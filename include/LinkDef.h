// SVN Info: $Id: LinkDef.h 542 2010-02-12 21:52:28Z werthm $

/*************************************************************************
 * Author: Dominik Werthmueller, 2007-2010
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// LinkDef.h                                                            //
//                                                                      //
// P2MKIN dictionary header file.                                       //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifdef __CINT__ 

// turn everything off
#pragma link off all globals; 
#pragma link off all classes; 
#pragma link off all functions; 
#pragma link off all typedef; 

#pragma link C++ nestedclasses; 
#pragma link C++ nestedtypedef; 

// P2MKIN namespaces
#pragma link C++ namespace TOGlobals; 
#pragma link C++ namespace TOSUtils; 

// classes
#pragma link C++ class TOMCMkinWriter+; 
#pragma link C++ class TOMCParticle+; 

#endif

