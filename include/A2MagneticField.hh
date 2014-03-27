#ifndef A2MagneticField_h
#define A2MagneticField_h 1

#include "G4MagneticField.hh"
#include "G4ThreeVector.hh"


class G4FieldManager;

class A2MagneticField:public G4MagneticField
{
  public:
    
    A2MagneticField(G4ThreeVector);
    A2MagneticField();      
    ~A2MagneticField(); 
    
    virtual void GetFieldValue(const G4double Point[4], G4double*) const;
    
    // Read field map from a .dat file and fill fFieldMap array
    virtual G4bool ReadFieldMap(const G4String&);
    
  protected:
    
    // min, max bounds for the magnetic field area
    G4double fPointMin[3];
    G4double fPointMax[3];
    
    // steps of coordinate-measuring and number of points
    G4double fPointStep[3];
    G4int fPointsN[3];
    
    // Data type for the magnetic induction vector
    typedef G4double A2Bvector[3];
    
    // Magnetic fieled map array
    A2Bvector*** fFieldMap;
    
    // Get index of the point "q" in an arithmetic progression with the first element "q0" and step "d"
    G4int GetPointIndex(const G4double& q, const G4double& q0, const G4double& d) const {return (q-q0)/d;}
    
    // Find the global Field Manager
//     G4FieldManager* GetGlobalFieldManager();
};

#endif
