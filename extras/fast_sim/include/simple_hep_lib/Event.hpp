#pragma once

#include "simple_hep_lib/Particle.hpp"
#include "simple_hep_lib/Jet.hpp"
//#include <boost/serialization/access.hpp>
//#include <boost/serialization/vector.hpp>
#include <algorithm>

namespace hep_simple_lib {


    /// Simple event class, separating into various classes of particle
    class Event {
    private:

      /// @name Serialization
      /// @todo This may have to be a Printer function... ask Ben about Printers
      //@{
      //friend class boost::serialization::access;
      /*
      template<class Archive>
      void serialize(Archive & ar, const unsigned int version) {
        ar & _photons;
        ar & _electrons;
        ar & _muons;
        ar & _taus;
        ar & _invisibles;
        ar & _jets;
        ar & _pmiss;
      }
      */
      //@}

      /// @name Internal particle / vector containers
      //@{

      /// @name Separate particle collections
      //@{
      /// @todo Do we really need to store invisibles, since they aren't
      /// experimentally resolveable, and are covered by the explicitly-set
      /// missing-mom?
      std::vector<Particle*> _photons, _electrons, _muons, _taus, _invisibles;
      //@}

      /// Jets collection (mutable to allow sorting)
      mutable std::vector<Jet*> _jets;

      /// Missing momentum vector
      P4 _pmiss;
      P4 _pmiss_truth;

      //@}

    public:

      /// @todo Need separate types of event (subclasses?) for what gets passed to
      /// detsim and to analysis?

      /// @todo This class should be able to do its own MET and SET calculations,
      /// identify photons, electrons, muons, taus, charged and neutral hadrons, B
      /// hadrons/quarks, and construct jets (?)


      /// @name Constructors
      //@{

      /// Default constructor
      Event() { clear(); }

      /// Constructor from a list of Particles
      Event(const std::vector<Particle*>& ps) {
        clear();
        add_particles(ps);
      }

      /// Destructor (cleans up all passed Particles and calculated Jets)
      ~Event() {
        clear();
      }

      //@}

      /// Empty the event's particle, jet and MET collections
      void clear() {
        /// @todo Prefer this form when we can use C++11's range-for
        // for (Particle* p : particles()) delete p;
        std::vector<Particle*> ps = particles();
        for (size_t i = 0; i < particles().size(); ++i) { delete ps[i]; }
        ps.clear();
        _photons.clear();
        _electrons.clear();
        _muons.clear();
        _taus.clear();
        /// @todo Really need to store invisibles when we have a dedicated MET variable?
        _invisibles.clear();

        /// @todo Prefer this form when we can use C++11's range-for
        // if (!_jets.empty()) for (Jet* j : jets()) delete j;
        if (!_jets.empty()) for (size_t i = 0; i < _jets.size(); ++i) delete _jets[i];
        _jets.clear();

        _pmiss.clear();
      }


      /// Add a final state particle to the event
      /// @todo Clarify ownership/lifetimes -- owned by outside code, or *to be cleaned up by the event on deletion*? THE LATTER
      /// @todo What about taus and Bs?
      /// @todo "Lock" at some point so that jet finding etc. only get done once
      void add_particle(Particle* p) {
        if (p->is_prompt()) {
          if (p->pid() == 22) _photons.push_back(p);
          if (abs(p->pid()) == 11) _electrons.push_back(p);
          if (abs(p->pid()) == 13) _muons.push_back(p);
          if (abs(p->pid()) == 15) _taus.push_back(p);
          if (abs(p->pid()) == 12 || abs(p->pid()) == 14 || abs(p->pid()) == 16 || abs(p->pid()) == 1000022) _invisibles.push_back(p);
        }
      }


      /// Add a collection of final state particles to the event
      /// @todo Should be vector<const Particle*>?
      void add_particles(const std::vector<Particle*>& ps) {
        for (size_t i = 0; i < ps.size(); ++i) add_particle(ps[i]);
      }


      /// Get all final state particles
      /// @todo Note the return by value: it's not efficient yet!
      std::vector<Particle*> particles() const {
        // Add together all the vectors of the different particle types
        std::vector<Particle*> rtn;
        // rtn.reserve(visible_particles().size() + _invisibles.size());
        rtn.reserve(_photons.size() + _electrons.size() + _muons.size() + _taus.size() + _invisibles.size());
        #define APPEND_VEC(vec) rtn.insert(rtn.end(), vec.begin(), vec.end())
        // APPEND_VEC(visible_particles());
        APPEND_VEC(_photons);
        APPEND_VEC(_electrons);
        APPEND_VEC(_muons);
        APPEND_VEC(_taus);
        APPEND_VEC(_invisibles);
        #undef APPEND_VEC
        return rtn;
        /// @todo Or use Boost range join to iterate over separate containers transparently... I like this
        /// @todo Cache, or otherwise think about efficiency since this gets called by the destructor
      }


      /// Get visible state particles
      /// @todo Note the return by value: it's not efficient yet!
      std::vector<Particle*> visible_particles() const {
        // Add together all the vectors of the different particle types
        std::vector<Particle*> rtn;
        rtn.reserve(_photons.size() + _electrons.size() + _muons.size() + _taus.size());
        #define APPEND_VEC(vec) rtn.insert(rtn.end(), vec.begin(), vec.end() )
        APPEND_VEC(_photons);
        APPEND_VEC(_electrons);
        APPEND_VEC(_muons);
        APPEND_VEC(_taus);
        #undef APPEND_VEC
        return rtn;
        /// @todo Add together all the vectors of the different particle types
        /// @todo Or use Boost range join to iterate over separate containers transparently... I like this
      }


      /// Get invisible final state particles
      const std::vector<Particle*>& invisible_particles() const {
        return _invisibles;
      }


      /// Get prompt electrons
      const std::vector<Particle*>& electrons() const {
        return _electrons;
      }


      /// Get prompt muons
      const std::vector<Particle*>& muons() const {
        return _muons;
      }


      /// Get prompt (hadronic) taus
      const std::vector<Particle*>& taus() const {
        return _taus;
      }


      /// Get prompt photons
      const std::vector<Particle*>& photons() const {
        return _photons;
      }


      /// @name Jets
      /// @todo Why the new'ing? Can we use references more?
      //@{

      /// @brief Get anti-kT 0.4 jets (not including charged leptons or photons)
      const std::vector<Jet*>& jets() const {
        std::sort(_jets.begin(), _jets.end(), _cmpPtDesc);
        return _jets;
      }

      /// @brief Set the jets collection
      ///
      /// The Jets should be new'd; Event will take ownership.
      void setJets(const std::vector<Jet*>& jets) {
        _jets = jets;
      }

      /// @brief Add a jet to the jets collection
      ///
      /// The Jet should be new'd; Event will take ownership.
      void addJet(Jet* j) {
        _jets.push_back(j);
      }

      //@}


      /// @name Missing energy
      //@{

      /// @brief Get the missing energy vector
      ///
      /// Not _necessarily_ the sum over momenta of final state invisibles
      const P4& missingmom() const {
        return _pmiss;
      }

      /// @brief Set the missing energy vector
      ///
      /// Not _necessarily_ the sum over momenta of final state invisibles
      void set_missingmom(const P4& pmiss) {
        _pmiss = pmiss;
      }

      /// Get the missing ET in GeV
      double met() const {
        return missingmom().pT();
      }

      ///
      /// the sum over momenta of final state invisibles
      const P4& missingmom_truth() const {
        return _pmiss_truth;
      }

      /// @brief Set the missing energy vector
      ///
      ///the sum over momenta of final state invisibles
      void set_missingmom_truth(const P4& pmiss) {
        _pmiss_truth = pmiss;
      }

      /// Get the missing ET in GeV calculated from the invisibles
      double met_truth() const {
        return missingmom_truth().pT();
      }


      //@}


    };


}
