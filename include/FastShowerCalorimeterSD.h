#ifndef EXME_CALORIMETER_SD_H
#define EXME_CALORIMETER_SD_H

//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file  FastShowerCalorimeterSD.h
/// \brief Definition of the FastShowerCalorimeterSD class
///
/// Geant4 ExampleN03 adapted to Virtual Monte Carlo: \n
/// Id: ExN03CalorimeterSD.hh,v 1.4 2002/01/09 17:24:11 ranjard Exp
/// GEANT4 tag Name: geant4-05-00
///
/// \author I. Hrivnacova; IPN, Orsay

#include <TNamed.h>
#include <TClonesArray.h>

class FastShowerDetectorConstruction;
class FastShowerCalorHit;
class TVirtualMC;

/// \ingroup EME
/// \brief The calorimeter sensitive detector
///
/// \date 06/03/2003
/// \author I. Hrivnacova; IPN, Orsay

class FastShowerCalorimeterSD : public TNamed
{
  public:
    FastShowerCalorimeterSD(const char* name,
                      FastShowerDetectorConstruction* detector);
    FastShowerCalorimeterSD(const FastShowerCalorimeterSD& origin,
                      FastShowerDetectorConstruction* detector);
    FastShowerCalorimeterSD();
    virtual ~FastShowerCalorimeterSD();

    // methods
    void    Initialize();
    Bool_t  ProcessHits();
    void    EndOfEvent();
    virtual void  Print(Option_t* option = "") const;
    void    PrintTotal() const;


    // set methods
    void SetVerboseLevel(Int_t level);

    // get methods
    FastShowerCalorHit* GetHit(Int_t i) const;

  private:
    // methods
    void  ResetHits();

    // data members
    TVirtualMC*    fMC;            ///< The VMC implementation
    FastShowerDetectorConstruction*  fDetector; ///< Detector construction
    TClonesArray*  fCalCollection; ///< Hits collection
    Int_t          fAbsorberVolId; ///< The absorber volume Id
    Int_t          fGapVolId;      ///< The gap volume Id
    Int_t          fVerboseLevel;  ///< Verbosity level

  ClassDef(FastShowerCalorimeterSD,1) //FastShowerCalorimeterSD

};

/// Set verbose level
/// \param level The new verbose level value
inline void FastShowerCalorimeterSD::SetVerboseLevel(Int_t level)
{ fVerboseLevel = level; }

#endif //EXME_CALORIMETER_SD_H
