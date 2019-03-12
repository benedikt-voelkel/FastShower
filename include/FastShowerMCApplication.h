#ifndef EXME_MC_APPLICATION_H
#define EXME_MC_APPLICATION_H

//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file FastShowerMCApplication.h
/// \brief Definition of the FastShowerMCApplication class
///
/// Geant4 ExampleN03 adapted to Virtual Monte Carlo
///
/// \author I. Hrivnacova; IPN, Orsay

#include <vector>
#include <unordered_map>
#include <string>
#include <functional>

#include <initializer_list>

#include <TVirtualMCApplication.h>

#include "FastShowerDetectorConstruction.h"
#include "FastShowerCalorimeterSD.h"

#include <TGeoUniformMagField.h>
#include <TMCVerbose.h>
#include <TH1D.h>
#include <TH2D.h>

class FastShowerMCStack;
class FastShowerPrimaryGenerator;


/// \ingroup EME
/// \brief Implementation of the TVirtualMCApplication
///
/// \date 06/03/2003
/// \author I. Hrivnacova; IPN, Orsay

class FastShowerMCApplication : public TVirtualMCApplication
{
  public:
    FastShowerMCApplication(const char* name,  const char *title,
                      Bool_t isMulti = kFALSE, Bool_t splitSimulation = kFALSE, Bool_t hasFastSim = kFALSE);
    FastShowerMCApplication();
    virtual ~FastShowerMCApplication();

    // methods
    void InitMC(const char *setup);
    void InitMC(std::initializer_list<const char*> setupMacros);
    void InitMC();
    void RunMC(Int_t nofEvents);
    void FinishRun();
    void ExportGeometry(const char* path) const;
    void ReadEvent(Int_t i);

    virtual TVirtualMCApplication* CloneForWorker() const;
    virtual void InitForWorker() const;
    virtual void FinishWorkerRun() const;

    virtual void ConstructGeometry();
    virtual void InitGeometry();
    virtual void AddParticles();
    virtual void AddIons();
    virtual void GeneratePrimaries();
    virtual void BeginEvent();
    virtual void BeginPrimary();
    virtual void PreTrack();
    virtual void Stepping();
    virtual void PostTrack();
    virtual void FinishPrimary();
    virtual void FinishEvent();

    // set methods
    void  SetPrintModulo(Int_t value);
    void  SetVerboseLevel(Int_t verboseLevel);
    void  SetControls(Bool_t isConstrols);
    void  SetField(Double_t bz);

    // get methods
    FastShowerDetectorConstruction* GetDetectorConstruction() const;
    FastShowerCalorimeterSD*        GetCalorimeterSD() const;
    FastShowerPrimaryGenerator*     GetPrimaryGenerator() const;

    // method for tests
    void SetOldGeometry(Bool_t oldGeometry = kTRUE);

    void WriteHistograms(const std::string& filename);

  private:
    // methods
    FastShowerMCApplication(const FastShowerMCApplication& origin);
    void RegisterStack() const;



    // data members
    Int_t                     fPrintModulo;     ///< The event modulus number to be printed
    Int_t                     fEventNo;         ///< Event counter
    TMCVerbose                fVerbose;         ///< VMC verbose helper
    FastShowerMCStack*              fStack;           ///< VMC stack
    FastShowerDetectorConstruction* fDetConstruction; ///< Dector construction
    FastShowerCalorimeterSD*        fCalorimeterSD;   ///< Calorimeter SD
    FastShowerPrimaryGenerator*     fPrimaryGenerator;///< Primary generator
    TGeoUniformMagField*      fMagField;        ///< Magnetic field
    Bool_t                    fOldGeometry;     ///< Option for geometry definition
    Bool_t                    fIsControls;      ///< Option to activate special controls
    Bool_t                    fIsMaster;        ///< If is on master thread
    Bool_t                    fIsMultiRun;      ///< Flag if having multiple engines
    Bool_t                    fSplitSimulation; ///< Split geometry given user criteria
    Bool_t                    fHasFastSim;      ///< For now just a helper flag
    Int_t                     fG3Id;            ///< engine ID of Geant3
    Int_t                     fG4Id;            ///< engine ID of Geant4
    Int_t                     fFastSimId;       ///< Id of registered fast sim
    std::vector<Int_t>        mNElectrons;      ///< Count number of electrons
    std::vector<Int_t>        mNPositrons;      ///< Count number of electrons
    std::vector<Int_t>        mNPhotons;      ///< Count number of electrons
    std::unordered_map<int, int> mStepsPerPdg;
    std::unordered_map<int, int> mBoundaryParticlesPerPdg;
    // All steps
    TH1D mStepsX;
    TH1D mStepsY;
    TH1D mStepsZ;

    // production vertex electrons
    TH1D mPVElectronsX;
    TH1D mPVElectronsY;
    TH1D mPVElectronsZ;

    // production mom electrons
    TH1D mPMomElectronsX;
    TH1D mPMomElectronsY;
    TH1D mPMomElectronsZ;

    /// engines vs volume
    TH2D mEngineVsVolume;

    /// Particles on boundary
    Bool_t fLeft;
    /// Store the proton energy when it leaves the calorimeter
    Double_t fProtonEnergy;
    Int_t fBoundaryParticles;
    std::vector<int> mBoundaryParticlesVec;
    TH1D fHistBoudaryX;
    TH1D fHistBoudaryY;
    TH1D fHistBoudaryZ;


    /// Energy deposited in calorimeter
    TH1D mHistDepEnergyLAr;

    // Engine vs. volume
    TH2D mHistDepEnergyLArProtonEnergy;




  ClassDef(FastShowerMCApplication,1)  //Interface to MonteCarlo application
};

// inline functions

/// Set the event modulus number to be printed
/// \param value  The new event modulus number value
inline void  FastShowerMCApplication::SetPrintModulo(Int_t value)
{ fPrintModulo = value; }

/// Set verbosity
/// \param verboseLevel  The new verbose level value
inline void  FastShowerMCApplication::SetVerboseLevel(Int_t verboseLevel)
{ fVerbose.SetLevel(verboseLevel); }

// Set magnetic field
// \param bz  The new field value in z
inline void  FastShowerMCApplication::SetField(Double_t bz)
{ fMagField->SetFieldValue(0., 0., bz); }

/// \return The detector construction
inline FastShowerDetectorConstruction* FastShowerMCApplication::GetDetectorConstruction() const
{ return fDetConstruction; }

/// \return The calorimeter sensitive detector
inline FastShowerCalorimeterSD* FastShowerMCApplication::GetCalorimeterSD() const
{ return fCalorimeterSD; }

/// \return The primary generator
inline FastShowerPrimaryGenerator* FastShowerMCApplication::GetPrimaryGenerator() const
{ return fPrimaryGenerator; }

/// Switch on/off the old geometry definition  (via VMC functions)
/// \param oldGeometry  If true, geometry definition via VMC functions
inline void FastShowerMCApplication::SetOldGeometry(Bool_t oldGeometry)
{ fOldGeometry = oldGeometry; }

/// Switch on/off special process controls
/// \param isControls  If true, special process controls setting is activated
inline void FastShowerMCApplication::SetControls(Bool_t isControls)
{ fIsControls = isControls; }

#endif //EXME_MC_APPLICATION_H
