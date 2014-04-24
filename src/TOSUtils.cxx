// SVN Info: $Id: TOSUtils.cxx 454 2009-06-21 19:24:30Z werthm $

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


#include "TOSUtils.h"

#if !defined(R__ALPHA) && !defined(R__SOLARIS) && !defined(R__ACC) && !defined(R__FBSD)
NamespaceImp(TOSUtils)
#endif


// Declare namespace member
Char_t* TOSUtils::fTmpCh = 0;


//______________________________________________________________________________
Int_t TOSUtils::IndexOf(const Char_t* s, Char_t c, UInt_t p)
{
    // Returns the position of the first occurence of the character c
    // in the string s after position p. Returns -1 if c was not found.

    const Char_t* pos = strchr(s+p, (Int_t)c);
    if (pos) return pos-s;
    else return -1;
}

//______________________________________________________________________________
Int_t TOSUtils::IndexOf(const Char_t* s1, const Char_t* s2, UInt_t p)
{
    // Returns the position of the first occurence of the string s2
    // in the string s1 after position p. Returns -1 if s2 was not found.

    const Char_t* pos = strstr(s1+p, s2);
    if (pos) return pos-s1;
    else return -1;
}

//______________________________________________________________________________
Int_t TOSUtils::LastIndexOf(const Char_t* s, Char_t c)
{
    // Returns the position of the last occurence of the character c
    // in the string s. Returns -1 if c was not found.
    
    const Char_t* pos = strrchr(s, (Int_t)c);
    if (pos) return pos-s;
    else return -1;
}

//______________________________________________________________________________
Char_t* TOSUtils::ExtractFileName(const Char_t* s)
{
    // Extracts the file name of a file given by its full Unix paths in
    // the string s.
    
    // Create output string if empty
    if (!fTmpCh) fTmpCh = new Char_t[fTmpChLength];
    
    // Search last slash
    Int_t pos = LastIndexOf(s, '/');

    // Return the same string or copy substring
    if (pos == -1) strcpy(fTmpCh, s);
    else strncpy(fTmpCh, s+pos+1, strlen(s)-pos);
    
    return fTmpCh;
}

//______________________________________________________________________________
Char_t* TOSUtils::ExtractPureFileName(const Char_t* s)
{
    // Extracts the file name and truncates the file ending of a file given 
    // by its full Unix path in the string s.
    
    // Create output string if empty
    if (!fTmpCh) fTmpCh = new Char_t[fTmpChLength];
   
    // Get the file name
    Char_t fn[256];
    strcpy(fn, ExtractFileName(s));
    
    // Search last dot
    Int_t pos = LastIndexOf(fn, '.');
    
    // Strip file ending
    if (pos == -1) strcpy(fTmpCh, fn);
    else
    {
        strncpy(fTmpCh, fn, pos);
        fTmpCh[pos] = '\0';
    }

    return fTmpCh;
}

//______________________________________________________________________________
Char_t* TOSUtils::Trim(const Char_t* s)
{
    // Removes the leading and trailing whitespace of the string s.

    Int_t l = strlen(s);
    Int_t posF = -1;
    Int_t posL = -1;

    // Create output string if empty
    if (!fTmpCh) fTmpCh = new Char_t[fTmpChLength];
    
    // Search first non-whitespace
    for (Int_t i = 0; i < l; i++)
    {
        // leave loop when first non-whitespace was found
        if (s[i] != ' ')
        {
            posF = i;
            break;
        }
    }

    // handle empty string
    if (posF == -1)
    {
        fTmpCh[0] = '\0';
        return fTmpCh;
    }
 
    // Search last non-whitespace
    for (Int_t i = l-1; i >= 0; i--)
    {
        // leave loop when last non-whitespace was found
        if (s[i] != ' ')
        {
            posL = i;
            break;
        }
    }

    // Copy trimmed string
    Int_t j = 0;
    for (Int_t i = posF; i <= posL; i++)
    {
        fTmpCh[j++] = s[i];
    }
    fTmpCh[j] = '\0';

    return fTmpCh;
}

//______________________________________________________________________________
Bool_t TOSUtils::IsComment(const Char_t* s)
{
    // Returns kTRUE if the string s is a comment, i.e. starts with a '#'
 
    // Trim string
    Char_t* st = Trim(s);
 
    // First character is a '#'
    if (st[0] == '#') return kTRUE;
    else return kFALSE;
}

//______________________________________________________________________________
Char_t* TOSUtils::FormatArrayList(Int_t n, Int_t* arr)
{
    // Returns a comma-separated list of all elements of the array 'arr' as 
    // a string.
    
    // Create output string if empty
    if (!fTmpCh) fTmpCh = new Char_t[fTmpChLength];

    // clear string
    fTmpCh[0] = '\0';
    
    // format list
    Char_t tmp[256];
    for (Int_t i = 0; i < n; i++)
    {
        if (i == n-1) sprintf(tmp, "%d", arr[i]);
        else sprintf(tmp, "%d, ", arr[i]);
        strcat(fTmpCh, tmp);
    }

    return fTmpCh;
}

//______________________________________________________________________________
Char_t* TOSUtils::FormatArrayList(Int_t n, UInt_t* arr)
{
    // Returns a comma-separated list of all elements of the array 'arr' as 
    // a string.
    
    // Create output string if empty
    if (!fTmpCh) fTmpCh = new Char_t[fTmpChLength];

    // clear string
    fTmpCh[0] = '\0';
    
    // format list
    Char_t tmp[256];
    for (Int_t i = 0; i < n; i++)
    {
        if (i == n-1) sprintf(tmp, "%d", arr[i]);
        else sprintf(tmp, "%d, ", arr[i]);
        strcat(fTmpCh, tmp);
    }

    return fTmpCh;
}

//______________________________________________________________________________
Char_t* TOSUtils::FormatArrayList(Int_t n, Double_t* arr)
{
    // Returns a comma-separated list of all elements of the array 'arr' as 
    // a string.
    
    // Create output string if empty
    if (!fTmpCh) fTmpCh = new Char_t[fTmpChLength];

    // clear string
    fTmpCh[0] = '\0';
    
    // format list
    Char_t tmp[256];
    for (Int_t i = 0; i < n; i++)
    {
        if (i == n-1) sprintf(tmp, "%e", arr[i]);
        else sprintf(tmp, "%e, ", arr[i]);
        strcat(fTmpCh, tmp);
    }

    return fTmpCh;
}

