//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  Lightweight higgs partial widths container
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Pat Scott
///          (p.scott@imperial.ac.uk)
///  \date 2016 Sep
///
///  *********************************************

#ifndef __higgs_couplings_table_hpp__
#define __higgs_couplings_table_hpp__

#include "gambit/Utils/util_types.hpp"
#include "gambit/Elements/decay_table.hpp"


namespace Gambit
{

  /// GAMBIT native higgs coupling table class.
  class HiggsCouplingsTable
  {
    private:

      /// SM neutral higgs decays
      /// @{
      DecayTable::Entry& neutral_decays_SM_array[max_neutral_higgses];
      std::map<str, DecayTable::Entry&> neutral_decays_SM_map;
      /// @}

      /// BSM higgs decays
      /// @{
      DecayTable::Entry& neutral_decays_array[max_neutral_higgses];
      DecayTable::Entry& charged_decays_array[max_charged_higgses];
      std::map<str, DecayTable::Entry&> neutral_decays_map;
      std::map<str, DecayTable::Entry&> charged_decays_map;
      /// @}

      /// t decays (as t can decay to h)
      DecayTable::Entry& t_decays;

    public:

      /// The maximal Higgs sector manageable by this class; update as needed.
      /// @{
      const max_neutral_higgses = 3;
      const max_charged_higgses = 1;
      /// @}

      /// CP of neutral higgses
      double CP[max_neutral_higgses];

      /// Effective couplings for neutral higgses
      /// @{
      double C_WW2[max_neutral_higgses];
      double C_ZZ2[max_neutral_higgses];
      double C_tt2[max_neutral_higgses];
      double C_bb2[max_neutral_higgses];
      double C_cc2[max_neutral_higgses];
      double C_tautau2[max_neutral_higgses]
      double C_gaga2[max_neutral_higgses];
      double C_gg2[max_neutral_higgses];
      double C_mumu2[max_neutral_higgses];
      double C_Zga2[max_neutral_higgses];
      double C_ss2[max_neutral_higgses];
      double C_hiZ2[max_neutral_higgses,max_neutral_higgses];
      /// @}

      /// Compute a neutral higgs effective coupling from the current two-body neutral higgs decays
      /// @{
      template <typename T>
      double compute_effective_coupling(int index, T& p1, T& p2)
      {
        if (index > max_neutral_higgses - 1) utils_error().raise(LOCAL_INFO, "Requested index beyond max_neutral_higgses.");
        // If channel is missing from either SM or BSM decays, return unity.
        if (!neutral_decays_SM_array[index].has_channel(p1, p2) or !neutral_decays_array[index].has_channel(p1, p2)) return 1.;
        double total_width_ratio = neutral_decays_array[index].width_in_GeV/neutral_decays_SM_array[index].width_in_GeV;
        double smbf = neutral_decays_SM_array[index].BF(p1, p2);
        if (smbf <= 0.) return 0.;
        double BF_ratio = neutral_decays_array[index].BF(p1, p2) / smbf;
        return total_width_ratio * BF_ratio;
      }
      template <typename T>
      double compute_effective_coupling(str name, T& p1, T& p2)
      {
        if (neutral_decays_SM_map.find(name) == neutral_decays_SM_map.end()) utils_error().raise(LOCAL_INFO, "Requested higgs not found.");
        // If channel is missing from either SM or BSM decays, return unity.
        if (!neutral_decays_SM_array[index].has_channel(p1, p2) or !neutral_decays_array[index].has_channel(p1, p2)) return 1.;
        if (neutral_decays_map.find(name) == neutral_decays_map.end()) utils_error().raise(LOCAL_INFO, "Requested higgs not found.");
        double total_width_ratio = neutral_decays_array_map[name].width_in_GeV/neutral_decays_SM_map[name].width_in_GeV;
        double smbf = neutral_decays_SM_map[name].BF(p1, p2);
        if (smbf <= 0.) return 0.;
        double BF_ratio = neutral_decays_map[name].BF(p1, p2) / smbf;
        return total_width_ratio * BF_ratio;
      }
      /// @}


      /// Assign decay entries to the various table components
      /// @{
      void set_neutral_decays_SM(int, const str&, DecayTable::Entry&);
      void set_neutral_decays(int, const str&, DecayTable::Entry&);
      void set_charged_decays(int, const str&, DecayTable::Entry&);
      void set_t_decays_SM(DecayTable::Entry&);
      /// @}

      /// Retrieve decay sets
      /// @{
      DecayTable::Entry&[] get_neutral_decays_SM_array(int) const;
      DecayTable::Entry& get_neutral_decays_SM(int) const;
      DecayTable::Entry& get_neutral_decays_SM(const str&) const;
      DecayTable::Entry&[] get_neutral_decays_array(int) const;
      DecayTable::Entry& get_neutral_decays(int) const;
      DecayTable::Entry& get_neutral_decays(const str&) const;
      DecayTable::Entry&[] get_charged_decays_array(int) const;
      DecayTable::Entry& get_charged_decays(int) const;
      DecayTable::Entry& get_charged_decays(const str&) const;
      DecayTable::Entry& get_t_decays() const;
      /// @}

  };


#endif //__higgs_couplings_table_hpp__