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

#include "gambit/Elements/higgs_couplings_table.hpp"


namespace Gambit
{

  /// Assign SM decay entries to neutral higgses
  void HiggsCouplingsTable::set_neutral_decays_SM(int index, const str& name, DecayTable::Entry& entry)
  {
    if (index > max_neutral_higgses - 1) utils_error().raise(LOCAL_INFO, "Requested index beyond max_neutral_higgses.");
    neutral_decays_SM_array[index] = entry;
    neutral_decays_SM_map[name] = entry;
  }

  /// Assign decay entries to neutral higgses
  void HiggsCouplingsTable::set_neutral_decays(int index, const str& name, DecayTable::Entry& entry)
  {
    if (index > max_neutral_higgses - 1) utils_error().raise(LOCAL_INFO, "Requested index beyond max_neutral_higgses.");
    neutral_decays_array[index] = entry;
    neutral_decays_map[name] = entry;
  }

  /// Assign decay entries to charged higgses
  void HiggsCouplingsTable::set_charged_decays(int index, const str& name, DecayTable::Entry& entry)
  {
    if (index > max_charged_higgses - 1) utils_error().raise(LOCAL_INFO, "Requested index beyond max_charged_higgses.");
    charged_decays_array[index] = entry;
    charged_decays_map[name] = entry;
  }

  /// Assign decay entries to top
  void HiggsCouplingsTable::set_t_decays(DecayTable::Entry& entry) { t_decays = entry; }

  /// Retrieve SM decays of all neutral higgses
  const DecayTable::Entry&[] HiggsCouplingsTable::get_neutral_decays_SM_array(int array_size) const
  {
    if (array_size > max_neutral_higgses) utils_error().raise(LOCAL_INFO, "Requested array size beyond max_neutral_higgses.");
    return neutral_decays_SM_array;
  }

  /// Retrieve SM decays of a specific neutral Higgs, by index
  const DecayTable::Entry& HiggsCouplingsTable::get_neutral_decays_SM(int index) const
  {
    if (index > max_neutral_higgses - 1) utils_error().raise(LOCAL_INFO, "Requested index beyond max_neutral_higgses.");
    return neutral_decays_SM_array[index];
  }

  /// Retrieve SM decays of a specific neutral Higgs, by name
  const DecayTable::Entry& HiggsCouplingsTable::get_neutral_decays_SM(const str& name) const
  {
    if (neutral_decays_SM_map.find(name) == neutral_decays_SM_map.end()) utils_error().raise(LOCAL_INFO, "Requested higgs not found.");
    return neutral_decays_SM_map.at(name);
  }

  /// Retrieve decays of all neutral higgses
  const DecayTable::Entry&[] HiggsCouplingsTable::get_neutral_decays_array(int array_size) const
  {
    if (array_size > max_neutral_higgses) utils_error().raise(LOCAL_INFO, "Requested arrray size beyond max_neutral_higgses.");
    return neutral_decays_array;
  }

  /// Retrieve decays of a specific neutral Higgs, by index
  const DecayTable::Entry& HiggsCouplingsTable::get_neutral_decays(int index) const
  {
    if (index > max_neutral_higgses - 1) utils_error().raise(LOCAL_INFO, "Requested index beyond max_neutral_higgses.");
    return neutral_decays_array[index];
  }

  /// Retrieve decays of a specific neutral Higgs, by name
  const DecayTable::Entry& HiggsCouplingsTable::get_neutral_decays(const str& name) const
  {
    if (neutral_decays_map.find(name) == neutral_decays_map.end()) utils_error().raise(LOCAL_INFO, "Requested higgs not found.");
    return neutral_decays_map.at(name);
  }

  /// Retrieve decays of all charged higgses
  const DecayTable::Entry&[] HiggsCouplingsTable::get_charged_decays_array(int array_size) const
  {
    if (array_size > max_charged_higgses) utils_error().raise(LOCAL_INFO, "Requested array size beyond max_charged_higgses.");
    return charged_decays_array;
  }

  /// Retrieve decays of a specific charged Higgs, by index
  const DecayTable::Entry& HiggsCouplingsTable::get_charged_decays(int index) const
  {
    if (index > max_charged_higgses - 1) utils_error().raise(LOCAL_INFO, "Requested index beyond max_charged_higgses.");
    return charged_decays_array[index];
  }

  /// Retrieve decays of a specific charged Higgs, by name
  const DecayTable::Entry& HiggsCouplingsTable::get_charged_decays(const str& name) const
  {
    if (charged_decays_map.find(name) == charged_decays_map.end()) utils_error().raise(LOCAL_INFO, "Requested higgs not found.");
    return charged_decays_map.at(name);
  }

  /// Retrieve decays of the top quark
  const DecayTable::Entry& HiggsCouplingsTable::get_t_decays() const
  {
    return t_decays;
  }

}

#endif //__higgs_couplings_table_hpp__