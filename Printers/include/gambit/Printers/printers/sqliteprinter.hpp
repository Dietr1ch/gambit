//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  SQLite printer class declaration
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///
///  \author Ben Farmer
///          (b.farmer@imperial.ac.uk)
///  \date 2018 Dec
///
///  *********************************************

#ifndef __sqliteprinter_hpp__
#define __sqliteprinter_hpp__

#include <vector>
#include <map>
#include <string>
#include <limits>
#include <sqlite3.h> // SQLite3 C interface 

// Gambit
#include "gambit/Printers/baseprinter.hpp"
#include "gambit/Utils/util_functions.hpp" // Need Utils::ci_less to make map find() functions case-insensitive, since SQLite is case insensitive

// Sequence of all types printable by the SQLitePrinter,
// except those that require special backend types
#define SQL_TYPES           \
  (int)                     \
  (uint)                    \
  (long)                    \
  (ulong)                   \
  (longlong)                \
  (ulonglong)               \
  (float)                   \
  (double)                  \
  (std::vector<double>)     \
  (map_str_dbl)             \
  (ModelParameters)         \
 
/*(std::vector<double>)     \
  (bool)                    \
  (map_str_dbl)             \
  (ModelParameters)         \
  (triplet<double>)         \
  (map_intpair_dbl)         \
  */
// Printable types that need to be excluded in
// standalone builds
#define SQL_MODULE_BACKEND_TYPES \

  /*(DM_nucleon_couplings)    \
    (Flav_KstarMuMu_obs)      \
  */

namespace Gambit
{
  namespace Printers
  {
    // Compute unique integer from two integers
    // We use this to turn MPI rank and point ID integers into an SQLite row ID
    inline std::size_t pairfunc(const std::size_t i, const std::size_t j);
 
    // Type of function pointer for SQLite callback function
    typedef int sql_callback_fptr(void*, int, char**, char**);
 
    /// The main printer class for output to SQLite database
    class SQLitePrinter : public BasePrinter
    {
      public:
        /// Constructor (for construction via inifile options)
        SQLitePrinter(const Options&, BasePrinter* const primary = NULL);

        /// Destructor
        ~SQLitePrinter() {}

        /// Virtual function overloads:
        ///@{

        // Initialisation function
        // Run by dependency resolver, which supplies the functors with a vector of VertexIDs whose requiresPrinting flags are set to true.
        void initialise(const std::vector<int>&);
        //void flush();
        void reset(bool force=false);
        void finalise(bool abnormal=false);

        // Get options required to construct a reader object that can read
        // the previous output of this printer.
        Options resume_reader_options();
 
        ///@}

        /// @{ Internal information required by auxilliary printer constructors

        std::string get_database_file();
        std::string get_table_name();
        std::size_t get_max_buffer_length();

        /// @} 

        ///@{ Print functions
        using BasePrinter::_print; // Tell compiler we are using some of the base class overloads of this on purpose.
        #define DECLARE_PRINT(r,data,i,elem) void _print(elem const&, const std::string&, const int, const unsigned int, const unsigned long);
        BOOST_PP_SEQ_FOR_EACH_I(DECLARE_PRINT, , SQL_TYPES)
        #ifndef SCANNER_STANDALONE
          BOOST_PP_SEQ_FOR_EACH_I(DECLARE_PRINT, , SQL_MODULE_BACKEND_TYPES)
        #endif
        #undef DECLARE_PRINT
        ///@}

         /// Helper print functions
        // Used to reduce repetition in definitions of virtual function overloads
        // (useful since there is no automatic type conversion possible)
        // This template should work for any simple numeric type
        template<class T>
        void template_print(T const& value, const std::string& label, const int /*IDcode*/, const unsigned int mpirank, const unsigned long pointID, const std::string& col_type)
        {
            typedef std::numeric_limits<T> lims;
            std::stringstream sdata;
            sdata.precision(lims::max_digits10);
            sdata<<value;
            insert_data(mpirank, pointID, label, col_type, sdata.str()); 
        } 

     private:
 
        #ifdef WITH_MPI
        // Gambit MPI communicator context for use within the SQLite printer system
        GMPI::Comm myComm;
        #endif

        std::size_t mpiRank;
        std::size_t mpiSize;
  
        // Pointer to primary printer object, for retrieving setup information.
        SQLitePrinter* primary_printer;

        // Path to output SQLite database file
        std::string database_file;

        // Name of data table to store results for this run
        std::string table_name;

        // Pointer to output sqlite3 database
        sqlite3* db;

        // Bool to record if we already have a database file open
        bool db_is_open;

        // Bool to record if an output table exists yet
        bool results_table_exists;

        // Set to record whether table columns have been created 
        std::map<std::string,std::string,Utils::ci_less> column_record; 

        /// @{ Buffer variable
        
        std::size_t max_buffer_length;        
        
        // Map from column name to (buffer column position, column type) pair
        std::map<std::string,std::pair<std::size_t,std::string>,Utils::ci_less> buffer_info;

        // "Header" vector for buffer, recording column names for each vector position
        std::vector<std::string> buffer_header;

        // Buffer for SQLite insertions. Kind of a 2D "array" of column data 
        // to be transformed into one big INSERT operation once full.
        std::map<std::size_t,std::vector<std::string>> transaction_data_buffer;

        /// @}

        // Determines whether output is new row insertions, or updates previously existing rows
        bool synchronised;

        // Verify that the outbase database is open and the results table exists
        void require_output_ready();
 
        // Open database and 'attach' it to this object
        // A database will be created if it doesn't exist
        void open_db(const std::string&);
    
        // Close the database file that is attached to this object
        void close_db();

        // Submit an SQL statement to the database
        int submit_sql(const std::string& local_info, const std::string& sqlstr, bool allow_fail=false, sql_callback_fptr callback=NULL, void* data=NULL, char **zErrMsg=NULL);
 
        // Create results table
        void make_table(const std::string&);

        // Check that a table column exists, and create it if needed
        void ensure_column_exists(const std::string&, const std::string&);

        // Create an SQL table insert operation for the current transaction_data_buffer
        // Modifies 'sql' stringstream in-place
        void turn_buffer_into_insert(std::stringstream& sql, const std::string& table);

        // Queue a table insert operation, and submit the queue if it is filled
        void insert_data(const unsigned int mpirank, const unsigned long pointID, const std::string& col_name, const std::string& col_type, const std::string& data);

        // Submit and clear insert operation queue 
        void dump_buffer();
        void dump_buffer_as_INSERT();
        void dump_buffer_as_UPDATE();
 
        // Delete all buffer data and reset all buffer variables
        void clear_buffer();
    };
    
    // Register printer so it can be constructed via inifile instructions
    // First argument is string label for inifile access, second is class from which to construct printer
    LOAD_PRINTER(sqlite, SQLitePrinter)

  }
}
#endif
