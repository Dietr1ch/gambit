// -*- C++ -*-
//
// This file is part of HEPUtils -- https://bitbucket.org/andybuckley/heputils
// Copyright (C) 2013-2017 Andy Buckley <andy.buckley@cern.ch>
//
// Embedding of HEPUtils code in other projects is permitted provided this
// notice is retained and the HEPUtils namespace and include path are changed.
//
#pragma once

#include "HEPUtils/MathUtils.h"
#include "HEPUtils/Vectors.h"

namespace HEPUtils {


  /// Simple jet class, encapsulating a momentum 4-vector and with some extra b-tag info
  /// @todo Derive from a PhysObj base class to centralise the momentum handling
  class Jet {

    /// @name Storage
    /// @{
    /// Momentum vector
    P4 _p4;
    /// B and C tags
    bool _isB, _isC;
    /// @}


  public:

    /// @name Constructors
    /// @{

    /// Constructor for a light jet without explicit constituents
    Jet(const P4& mom, bool isB=false, bool isC=false)
      : _p4(mom), _isB(isB), _isC(isC) {  }

    /// "Cartesian" constructor
    Jet(double px, double py, double pz, double E, bool isB=false, bool isC=false)
      : _p4(px, py, pz, E), _isB(isB), _isC(isC) {  }

    /// @}


    /// @name Implicit casts
    /// @{

    operator const P4& () const { return mom(); }

    operator const P4* () const { return &mom(); }

    /// @}


    /// @name Momentum
    /// @{

    /// Get the 4 vector
    const P4& mom() const { return _p4; }
    /// Set the 4 vector
    void set_mom(const P4& p4) { _p4 = p4; }


    /// Get the mass (of the 4 vector)
    double mass() const { return _p4.m(); }
    /// Set the mass (of the 4 vector)
    void set_mass(double mass) { _p4.setM(mass); }


    /// Get the pseudorapidity
    double eta() const { return mom().eta(); }

    /// Get the abs pseudorapidity
    double abseta() const { return mom().abseta(); }

    /// Get the rapidity
    double rap() const { return mom().rap(); }

    /// Get the abs rapidity
    double absrap() const { return mom().absrap(); }

    /// Get the azimuthal angle
    double phi() const { return mom().phi(); }

    /// Get the energy
    double E() const { return mom().E(); }

    /// Get the squared transverse momentum
    double pT2() const { return mom().pT2(); }

    /// Get the squared transverse momentum
    double pT() const { return mom().pT(); }

    /// @}


    /// @name Tagging
    /// @{

    /// Is this particle tagged as a b?
    bool btag() const { return _isB; }
    /// Set BTag value
    void set_btag(bool isb) { _isB = isb; }

    /// Is this particle tagged as a c?
    /// @note Can be simultaneously btag()'d -- analyses should probably only use if fallback from b-tag.
    bool ctag() const { return /* !btag() && */ _isC; }
    /// Set CTag value
    void set_ctag(bool isc) { _isC = isc; }

    /// @todo Generalize for charm tags, tau tags, multiple tags of a single type?

    /// @}

  };


  /// @defgroup jet_const Jet constness conversion
  /// @{

  /// Convenience Jet cast to const
  inline const Jet* mkconst(Jet* jet) {
    return const_cast<const Jet*>(jet);
  }

  /// Convenience Jet cast to non-const
  inline Jet* mkunconst(const Jet* cjet) {
    return const_cast<Jet*>(cjet);
  }

  /// Get a reference to a vector of Jets, with each member const
  inline std::vector<const Jet*>& mkconst(const std::vector<Jet*>& jets) {
    return * (std::vector<const Jet*>*)(void*) (&jets);
  }

  /// Get a reference to a vector of Jets, with each member non-const
  inline std::vector<Jet*>& mkunconst(const std::vector<const Jet*>& cjets) {
    return * (std::vector<Jet*>*) (void*) (&cjets);
  }

  /// @}



  // /// Function/functor for container<const Jet*> sorting (cf. std::less)
  // inline bool _cmpPtDescJet(const Jet* a, const Jet* b) {
  //   return a->pT2() >= b->pT2();
  // }


}
