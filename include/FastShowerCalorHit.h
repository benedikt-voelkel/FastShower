#ifndef EXME_CALOR_HIT_H
#define EXME_CALOR_HIT_H

//------------------------------------------------
// The Virtual Monte Carlo examples
// Copyright (C) 2007 - 2014 Ivana Hrivnacova
// All rights reserved.
//
// For the licensing terms see geant4_vmc/LICENSE.
// Contact: root-vmc@cern.ch
//-------------------------------------------------

/// \file FastShowerCalorHit.h
/// \brief Definition of the FastShowerCalorHit class
///
/// Geant4 ExampleN03 adapted to Virtual Monte Carlo: \n
/// Id: ExN03CalorHit.hh,v 1.5 2002/01/09 17:24:11 ranjard Exp \n
/// GEANT4 tag Name: geant4-05-00
///
/// \author I. Hrivnacova; IPN, Orsay

#include <TObject.h>
#include <TVector3.h>

/// \ingroup EME
/// \brief The calorimeter hit
///
/// \date 06/03/2003
/// \author I. Hrivnacova; IPN, Orsay

class FastShowerCalorHit : public TObject
{
  public:
    FastShowerCalorHit();
    virtual ~FastShowerCalorHit();

    // methods
    virtual void Print(Option_t* option = "") const;

         /// Add energy deposit and track length in the absorber
         /// \param de  Enery deposit
         /// \param dl  Track length
    void AddAbs(Double_t de, Double_t dl) { fEdepAbs += de; fTrackLengthAbs += dl; }

         /// Add energy deposit and track length in the gap
         /// \param de  Enery deposit
         /// \param dl  Track length
    void AddGap(Double_t de, Double_t dl) { fEdepGap += de; fTrackLengthGap += dl; }

    void Reset();

    // get methods
             /// \return The energy deposit in the absorber
    Double_t GetEdepAbs() { return fEdepAbs; };
             /// \return The track length in the absorber
    Double_t GetTrakAbs() { return fTrackLengthAbs; };
             /// \return The energy deposit in the gap
    Double_t GetEdepGap() { return fEdepGap; };
             /// \return The track length in the gap
    Double_t GetTrakGap() { return fTrackLengthGap; };

  private:
    Double_t fEdepAbs;        ///< Energy deposit in the absorber
    Double_t fTrackLengthAbs; ///< Track length in the absorber
    Double_t fEdepGap;        ///< Energy deposit in the gap
    Double_t fTrackLengthGap; ///< Track length in the gap

  ClassDef(FastShowerCalorHit,1) //FastShowerCalorHit
};

#endif //EXME_CALOR_HIT_H
