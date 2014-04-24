// SVN Info: $Id: TOMCParticle.cxx 546 2010-02-18 22:00:45Z werthm $

/*************************************************************************
 * Author: Dominik Werthmueller, 2008
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TOMCParticle                                                         //
//                                                                      //
// Class representing a particle in the MC simulation.                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "TOMCParticle.h"

ClassImp(TOMCParticle)


//______________________________________________________________________________
TOMCParticle::TOMCParticle(TParticlePDG* inPDG, ParticleDecayMode_t inDecayMode, Bool_t initDecay)
    : TObject()
{
    // Constructor using the PDG particle definition 'inPDG' and the decay mode
    // 'inDecayMode'.
    // Set the random number generator seed if 'initDecay' is kTRUE (default).

    fPDG       = inPDG;
    fP4        = new TLorentzVector(0., 0., 0., 0.);
    fMassDistr = 0;

    SetUpDecay(inDecayMode);
    
    // set machine clock based random seed to the standart
    // ROOT random number generator (used by TGenPhaseSpace)
    if (initDecay) gRandom->SetSeed(0);
}

//______________________________________________________________________________
TOMCParticle::TOMCParticle(const Char_t* inName, ParticleDecayMode_t inDecayMode, Bool_t initDecay)
    : TObject()
{
    // Constructor using the particle name 'inName' and the decay mode
    // 'inDecayMode'.
    // Set the random number generator seed if 'initDecay' is kTRUE (default).

    fPDG = TOGlobals::GetPDG_DB()->GetParticle(inName);
    fP4        = new TLorentzVector(0., 0., 0., 0.);
    fMassDistr = 0;
    
    SetUpDecay(inDecayMode);
    
    // set machine clock based random seed to the standart
    // ROOT random number generator (used by TGenPhaseSpace)
    if (initDecay) gRandom->SetSeed(0);
}

//______________________________________________________________________________
TOMCParticle::TOMCParticle(Int_t inID, Bool_t isGeant3_ID, ParticleDecayMode_t inDecayMode, Bool_t initDecay)
    : TObject()
{
    // Constructor using the PDG or the GEANT3 particle ID 'inID' and the decay mode
    // 'inDecayMode'.
    // Set the random number generator seed if 'initDecay' is kTRUE (default).

    if (isGeant3_ID) fPDG = TOGlobals::GetPDG_DB()->GetParticle(TOGlobals::GetPDG_DB()->ConvertGeant3ToPdg(inID));
    else fPDG = TOGlobals::GetPDG_DB()->GetParticle(inID);
    fP4        = new TLorentzVector(0., 0., 0., 0.);
    fMassDistr = 0;
    
    SetUpDecay(inDecayMode);
    
    // set machine clock based random seed to the standart
    // ROOT random number generator (used by TGenPhaseSpace)
    if (initDecay) gRandom->SetSeed(0);
}

//______________________________________________________________________________
TOMCParticle::~TOMCParticle()
{
    // Destructor.
 
    if (fP4)    delete fP4;
    if (fNDecay) 
    {
        for (Int_t i = 0; i < fNDecay; i++) delete fDecay[i];
        delete [] fDecay;
    }
}

//______________________________________________________________________________
Int_t TOMCParticle::GetGeant3_ID() const 
{
    // Return the GEANT3 particle ID of this particle.
    
    // handle self-defined particles
    if (fPDG->PdgCode() == TOGlobals::kDeuteronID) return fPDG->TrackingCode();
    else return TOGlobals::GetPDG_DB()->ConvertPdgToGeant3(fPDG->PdgCode()); 
}

//______________________________________________________________________________
void TOMCParticle::SetUpDecay(ParticleDecayMode_t inDecayMode)
{
    // Configure this class to use the decay mode 'inDecayMode'.

    // set class decay mode member
    fDecayMode = inDecayMode;
    
    // configure decay
    switch (fDecayMode)
    {
        // no decay: set decay particle list to zero
        case kNoDecay:
        {
            fNDecay = 0;
            fDecay  = 0;
            break;
        }
    }
}

//______________________________________________________________________________
Int_t TOMCParticle::GetTotalNDecayParticles() const
{
    // Returns the total number of particles of this particle's decay chain
    // (recursive method).

    // no decay
    if (fDecayMode == kNoDecay) return 0;
    
    // recursive search through decay chain
    Int_t npart = 0;
    for (Int_t i = 0; i < fNDecay; i++)
    {
        // get decay particle and count it
        TOMCParticle* p = GetDecayParticle(i);
        npart++;

        // count decay particles of this decay particle
        npart += p->GetTotalNDecayParticles();
    }
    
    return npart;
}

//______________________________________________________________________________
Bool_t TOMCParticle::CalculateDecay()
{
    // Calculate a phase-space decay of this particle whole decay chain
    // (recursive method). Return kFALSE if there was an error during the
    // calculation, e.g. insufficient center-of-mass energy for a decay.
    //
    // ATTENTION: All input energies are assumed to be in GeV and all output energies
    //            will be set in GeV!

    // no decay
    if (fDecayMode == kNoDecay) return kTRUE;

    // set masses of decay particles
    // choose random mass from distribution if the particle is a Rootino
    // and a mass distribution was specified.
    Double_t masses[fNDecay];
    for (Int_t i = 0; i < fNDecay; i++) 
    {
        // get decay particle
        TOMCParticle* decayPart = GetDecayParticle(i);

        // check for Rootinos
        if (decayPart->GetPDG_ID()) 
        {
            masses[i] = decayPart->GetPDGMassGeV();
        }
        else
        {
            // check for mass distribution, set random mass [GeV]
            if (decayPart->GetMassDistribution()) 
            {
                masses[i] = 0.001 * decayPart->GetMassDistribution()->GetRandom();
            }
            else 
            {
                Error("CalculateDecay", "No mass distribution found for decay Rootino");
                return kFALSE;
            }
        }
    }

    // calculate decay
    TGenPhaseSpace decayGen;
    if (decayGen.SetDecay(*fP4, fNDecay, masses))
    {
        // generate decay
        decayGen.Generate();

        // set 4-vectors of decay products
        for (Int_t i = 0; i < fNDecay; i++) GetDecayParticle(i)->SetP4(decayGen.GetDecay(i));
    }
    else 
    {
        // decay could not be calculated (forbidden by kinematics)
        return kFALSE;
    }

    // calculate decay of each decay product
    for (Int_t i = 0; i < fNDecay; i++) 
    {
        // abort decay chain calculation if there was an error
        if (!GetDecayParticle(i)->CalculateDecay()) return kFALSE;
    }

    // calculation is ok here
    return kTRUE;
}

//______________________________________________________________________________
TOMCParticle* TOMCParticle::GetDecayParticle(Int_t index) const
{
    // Return the decay particle with index 'index'. Return 0 if no such 
    // particle could be found.

    // check index range
    if (index < 0 || index >= fNDecay)
    {
        Error("GetDecayParticle", "Decay particle index %d out of bounds (number of decay particles: %d)", index, fNDecay);
        return 0;
    }
    else
    {
        return fDecay[index];
    }
}

