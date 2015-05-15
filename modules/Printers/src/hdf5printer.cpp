//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  HDF5 interface printer class member function definitions
///
/// 
///  Design strategy:
///  
///  Gambit operates in an iterative fashion, collecting likelihood samples in a
///  multidimensional space. Along with the likelihoods, we also want to store
///  other information collected at each parameter space point. Naively the results
///  of this search could be stored in a record array, however Gambit operates at
///  too high a level to determine the type information necessary to define the
///  type of the record we want to store in any given scan.
///  
///  To solve this problem, while maintaining the ability to store a wide variety
///  of types, the Gambit HDF5 interface is implemented with the following strategy:
///  
///   - Every vertex ID becomes associated with its own, independent, HDF5 dataset,
///     whose type is suited to that vertex (i.e. Gambit functor). This also means
///     that vertices are permitted to have variable-length arrays as their record
///     type (useful for perhaps storing event data of some kind). It is better
///     for later analysis to store the data this way anyway, since one almost
///     always wants one or two or several "columns" of data at a time, rather than
///     every column (complete record) for several parameter points. We effectively
///     store each "column" separately, which makes retrieval of just one "column" 
///     very fast.
///  
///   - Each dataset contains a boolean field which indicates whether a record has
///     been filled or not (some points fail to produce results in all functors).
///  
///   - When the scanner moves to a new point, all the datasets have an entry added
///     (via their individual buffers). The boolean flag is set to zero 
///     (indicating no result) if no result was provided before this. This keeps
///     all the datasets synchronised, so that e.g. record 5 in all datasets refers
///     to the same parameter space point. 
///  
///   - The rank 0 process handles all dataset access, which allows it to keep the
///     datasets synchronised even when point information is coming in from several
///     points at once.
///  
///   -- To elaborate on the above two points further:
///      - the MPI handling is a little tricky here. Every type needs to know how to
///        serialise itself for transmission to the rank 0 process. Each process can
///        keep a series of buffers (instead of the actual datasets) synchronised
///        by the above method, and when they are full this can trigger the MPI send.
///      - The MPI receive must happen when the rank 0 process knows that it has 
///        completed one of its own points, so that a clean write to all the datasets
///        can occur. 
///  
///  In order to handle the auxilliary printers, i.e. the case where extra information
///  about points is added after the scan has moved on, the following strategy is used:
///  
///   - The auxilliary printers communicate with the main printer, to let it know what
///     information will appear in the future. The main printer then creates datasets
///     for this future information, and keeps them (and their boolean flags) in
///     lockstep with all the other datasets. 
///  
///   - When the auxilliary printer decides that it wants to print its information, a 
///     lookup is done in the datasets containing the pointID and rank information, to
///     figure out which absolute index the auxilliary data belongs to. The pre-existing
///     dataset created by the main printer is then updated accordingly. It might not
///     be possible to allow variable length arrays for this auxilliary data.
///  
///   - It may occur that the above lookup fails, because the auxilliary data has been
///     computed for a point which hasn't yet been delivered to the rank 0 process (i.e.
///     the main printer data is still in the buffer of another process). In this case
///     the auxilliary data will remain in a buffer (with its associated pointID and rank)
///     and the lookup will be attempted again at a later stage (unless the auxilliary
///     printer signals that the auxilliary data is going to be completely overwritten, in
///     which case the buffer is simply erased). 
///  
/// 
///  *********************************************
///
///  Authors (add name and date if you modify):
///   
///  \author Ben Farmer
///          (benjamin.farmer@fysik.su.se)
///  \date 2015 May
///
///  *********************************************


// Standard libraries
#include <map>
#include <vector>
#include <algorithm>
#include <ios>
#include <sstream>
#include <fstream>
#include <iomanip>

// Gambit
#include "gambit/Printers/printers/hdf5printer.hpp"
#include "gambit/Printers/hdf5tools.hpp"
#include "gambit/Core/error_handlers.hpp"
#include "gambit/Utils/stream_overloads.hpp"
#include "gambit/Utils/util_functions.hpp"

// MPI bindings
#include "gambit/Utils/mpiwrapper.hpp"

// Switch for debugging output (manual at the moment)

#define DEBUG_MODE

#ifdef DEBUG_MODE 
  #define DBUG(x) x
#else 
  #define DBUG(x)
#endif


// Code!
namespace Gambit
{

  namespace Printers 
  {
   
     // We are going to have to combine this data with information from the 
     // scanners (using the auxilliary printers). In order to do this efficiently,
     // we will store the pointIDs and ranks in a dataset seperate from the
     // bulk of the data (but correlated with it) so that we can quickly search
     // for records by their pointID and rank, and then write new data to them.
     //
     // NOTE: will have to change the auxilliary printers a bit, so that they
     // communicate what they intend to write back to the main printer... or something.


    /// @{ H5P_LocalBufferManager member functions

    template<class BuffType>
    void H5P_LocalBufferManager<BuffType>::init(HDF5Printer* p)
    {
       if(p==NULL)
       {
          std::ostringstream errmsg;
          errmsg << "Error! Tried to initialise a H5P_LocalBufferManager with a null pointer! Need an actual HDF5Printer object in order to work. This is a bug in the HDF5Printer class, please report it.";
          printer_error().raise(LOCAL_INFO, errmsg.str());
       }
       if(not ready()) {
          printer = p; 
       } else {
          std::ostringstream errmsg;
          errmsg << "Error! Tried to initialise a H5P_LocalBufferManager twice! This is a bug in the HDF5Printer class, please report it.";
          printer_error().raise(LOCAL_INFO, errmsg.str());
       }
    }    

    template<class BuffType>
    BuffType& H5P_LocalBufferManager<BuffType>::get_buffer(const int vertexID, const uint aux_i, const std::string& label) 
    {
       VBIDpair key = std::make_pair(vertexID,aux_i);
       typename std::map<VBIDpair, BuffType>::iterator it = local_buffers.find(key);
       if( it == local_buffers.end() ) 
       {
          // No buffer (should) exists for this output stream yet, so make one
          error_if_key_exists(local_buffers, key, "local_buffers");
          local_buffers[key] = BuffType(location,label/*deconstruct?*/,vertexID);

          // Get the new buffer back out of the map
          it = local_buffers.find(key);

          // Add a pointer to the new buffer to the full list as well
          if(printer!=NULL) {
            printer->insert_buffer( key, it->second );
          } else {
            std::ostringstream errmsg;
            errmsg << "Error! Tried to use H5P_LocalBufferManager before initialising it! This is a bug in the HDF5Printer class, please report it.";
            printer_error().raise(LOCAL_INFO, errmsg.str());
          }
       }
       return it->second; 
    }
  
    /// @}


    /// @{ HDF5Printer member functions
     
    // Constructor
    HDF5Printer::HDF5Printer(const Options& options)
      : printer_name("Primary printer")
    {
      DBUG( std::cout << "Constructing Primary HDF5Printer object..." << std::endl; )
 
      std::string file = options.getValue<std::string>("output_file");
      std::string group = options.getValue<std::string>("group");

      // Open HDF5 file (create if non-existant)
      Utils::ensure_path_exists(file);
      fileptr = HDF5::openFile(file);
 
      // Open requested group (creating it plus parents if needed)
      groupptr = HDF5::openGroup(fileptr,group);

      // Set the target dataset write location to the chosen group
      location = groupptr; 
   }
 
    /// Auxiliary mode constructor 
    HDF5Printer::HDF5Printer(const Options& options, std::string& name, bool globalIN)
      : printer_name(name)
    {
      // Could set these things via options also if we like.
      DBUG( std::cout << "Constructing Auxilliary HDF5Printer object (with name=\""<<printer_name<<"\")..." << std::endl; )
    }
 
    /// Destructor
    // Overload the base class virtual destructor
    HDF5Printer::~HDF5Printer()
    {
      // Make sure buffer is completely written to disk
      DBUG( std::cout << "Destructing HDF5Printer object (with name=\""<<printer_name<<"\")..." << std::endl; )
      //dump_buffer(true); // Buffers should dump themselves as they are destructed
      //DBUG( std::cout << "Buffer (of HDF5Printer with name=\""<<printer_name<<"\") successfully dumped..." << std::endl; )
    }
 
    /// Initialisation function
    // Run by dependency resolver, which supplies the functors with a vector of VertexIDs whose requiresPrinting flags are set to true.
    void HDF5Printer::initialise(const std::vector<int>& printmevec)
    {
      // Currently don't seem to need this... could use it to check if all VertexID's have submitted print requests.
    }

    void HDF5Printer::flush() {}

    /// Delete contents of output file (to be replaced/updated) and erase everything in the buffer
    void HDF5Printer::reset() {} 
  
    /// Retrieve MPI rank
    int HDF5Printer::getRank() {return myRank;}

    /// Add a pointer to a new buffer to the global list
    void HDF5Printer::insert_buffer(VBIDpair& key, VertexBufferBase& newbuffer)
    {
       error_if_key_exists(all_buffers, key, "all_buffers");
       all_buffers[key] = &newbuffer;
    }

    /// Add PPIDpair to global index list
    void HDF5Printer::add_PPID_to_list(const PPIDpair& ppid)
    {
      // TODO: is this going to cause memory issues? may have to rethink...

      // Check if it is in the lookup map already
      std::map<PPIDpair, ulong>::const_iterator it = global_index_lookup.find(ppid);
      if ( it != global_index_lookup.end() ) {
         std::ostringstream errmsg;
         errmsg << "Error! Supplied PPID already exists in global_index_lookup map! It should only be added once, so there is a bug in HDF5Printer. Please report this error.";
         printer_error().raise(LOCAL_INFO, errmsg.str());
      }
      
      // Ok, now safe to add it 
      global_index_lookup[ppid] = reverse_global_index_lookup.size();
      reverse_global_index_lookup.push_back(ppid);
    } 


    /// Function to ensure buffers are all synchronised to the same absolute position
    // Will move the "write heads" of all buffers to "current_dset_position"
    // This should only required one "skip_append" command at most to each buffer; something
    // went wrong if more are required.
    void HDF5Printer::synchronise_buffers()
    {
      // Determine the new sync position
      const ulong sync_pos = reverse_global_index_lookup.size()-1;  

      // Cycle through all buffers and tell them to ensure they are at the right position
      // The buffers should throw an error if we are accidentally telling them to go backwards
      // or skip too many points or anything they can't do.
      // Here though we should only be moving them forward by one position.
      for (BaseBufferMap::iterator it = all_buffers.begin(); it != all_buffers.end(); it++)
      {
        it->second->synchronise_output_to_position(sync_pos);
      }
    } 

    /// Check whether printing to a new parameter space point is about to occur
    // and perform adjustments needed to prepare the printer.
    void HDF5Printer::check_for_new_point(const ulong candidate_newpoint, const uint mpirank)
    {
       // Check that we are still writing to the same output "slot" as during the last print call
       if(candidate_newpoint!=lastPointID)
       {
         // Yep the scanner has moved on, at least as far as the current process sees
         lastPointID = candidate_newpoint;

         // In principle this should only happen once per pointID/process rank pair.
         // So add the new pair to the lookup table and register that we have moved to
         // a new parameter space point.
         add_PPID_to_list(std::make_pair(candidate_newpoint,mpirank));
 
         // Make sure all the buffers are synchronised at the new position.
         synchronise_buffers();          
       }
    }


    // PRINT FUNCTIONS
    //----------------------------
    // Need to define one of these for every type we want to print!
    // Could use macros again to generate identical print functions 
    // for all types that have a << operator already defined.
  
    void HDF5Printer::print(std::vector<double> const& value, const std::string& label, const int vID, const uint mpirank, const ulong pointID)
    {
       // We will write to several 'double' buffers, rather than a single vector buffer.
       // Change this once a vector buffer is actually available
       typedef VertexBufferNumeric1D_HDF5<double,BUFFERLENGTH> BuffType;
      
       // Retrieve the buffer manager for buffers with this type
       typedef H5P_LocalBufferManager<BuffType> BuffMan;
       BuffMan& buffer_manager = get_mybuffermanager<BuffType>(pointID,mpirank);

       //std::vector<std::string> labels;
       //labels.reserve(value.size());

       for(unsigned int i=0;i<value.size();i++)
       {
         // Might want to find some way to avoid doing this every single loop, seems kind of wasteful.
         std::stringstream ss;
         ss<<label<<"["<<i<<"]"; 
         //labels.push_back(ss.str());

         // Write to each buffer
         buffer_manager.get_buffer(vID, i, ss.str()).append(value[i]);
       }
    }
   
    void HDF5Printer::print(ModelParameters const& value, const std::string& label, const int vID, const uint mpirank, const ulong pointID)
    {
       // We will write to several 'double' buffers, since modelparameters are often retrieved separately
       typedef VertexBufferNumeric1D_HDF5<double,BUFFERLENGTH> BuffType;
      
       // Retrieve the buffer manager for buffers with this type
       typedef H5P_LocalBufferManager<BuffType> BuffMan;
       BuffMan& buffer_manager = get_mybuffermanager<BuffType>(pointID,mpirank);

       std::map<std::string, double> parameter_map = value.getValues();
    
       uint i=0; // index for each buffer 
       for (std::map<std::string, double>::iterator 
         it = parameter_map.begin(); it != parameter_map.end(); it++)
       {
         std::stringstream ss;
         ss<<label<<"::"<<it->first;

         // Write to each buffer
         buffer_manager.get_buffer(vID, i, ss.str()).append(it->second);
         i++;
       }
    }
 
    /// @}  
   
  } // end namespace printers
} // end namespace Gambit

#undef DBUG
#undef DEBUG_MODE
