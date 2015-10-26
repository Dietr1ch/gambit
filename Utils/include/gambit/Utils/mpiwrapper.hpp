//   GAMBIT: Global and Modular BSM Inference Tool
//   *********************************************
///  \file
///
///  A simple C++ wrapper for the MPI C bindings.
///  It is by no means comprehensive, and is just
///  intended to simplify some MPI commands within
///  Gambit.
///
///  Extend as needed.
///
///  Note: Some C++ bindings already exist, but
///  it appears that they are deprecated in really
///  new versions of the MPI standard, and don't
///  offer functionality over the C bindings
///  anyway. I have nevertheless mirrored the names
///  of some of the deprecated C++ bindings, in
///  case it helps some people who were used to
///  them.
///
///  There is also boost/mpi, but it is a compiled
///  library and we have been avoiding those.  
///
///  You can remove the error handlers if you want
///  these wrappers to be independent of GAMBIT.
///
///  *********************************************
///
///  Authors (add name and date if you modify):
///   
///  \author Ben Farmer
///          (benjamin.farmer@fysik.su.se)
///  \date 2015 Apr
///
///  \author Pat Scott
///          (p.scott@imperial.ac.uk)
///  \date 2015 Jun
///
///  *********************************************

// The WITH_MPI macro determines whether MPI is used or not.
// It is defined at compile time by cmake with -DWITH_MPI.
// The contents of this file are ignored if MPI is disabled.

#ifdef WITH_MPI
#ifndef __mpiwrapper_hpp__
#define __mpiwrapper_hpp__

#include <sstream>
#include <iostream>
#include <type_traits>
#include <chrono>

#include "gambit/Core/error_handlers.hpp"

#include <mpi.h>
#include <boost/utility/enable_if.hpp>

/// Provide template specialisation of get_mpi_data_type only if the requested type hasn't been used to define one already.
#define SPECIALISE_MPI_DATA_TYPE_IF_NEEDED(TYPEDEFD_TYPE, RETURN_MPI_TYPE)                                                   \
      template<typename T>                                                                                                   \
      struct get_mpi_data_type<T, typename boost::enable_if_c< std::is_same<T, TYPEDEFD_TYPE>::value                      && \
                                                               !std::is_same<char, TYPEDEFD_TYPE>::value                  && \
                                                               !std::is_same<short, TYPEDEFD_TYPE>::value                 && \
                                                               !std::is_same<int, TYPEDEFD_TYPE>::value                   && \
                                                               !std::is_same<long, TYPEDEFD_TYPE>::value                  && \
                                                               !std::is_same<long long, TYPEDEFD_TYPE>::value             && \
                                                               !std::is_same<unsigned char, TYPEDEFD_TYPE>::value         && \
                                                               !std::is_same<unsigned short, TYPEDEFD_TYPE>::value        && \
                                                               !std::is_same<unsigned int, TYPEDEFD_TYPE>::value          && \
                                                               !std::is_same<unsigned long, TYPEDEFD_TYPE>::value         && \
                                                               !std::is_same<unsigned long long, TYPEDEFD_TYPE>::value    && \
                                                               !std::is_same<float, TYPEDEFD_TYPE>::value                 && \
                                                               !std::is_same<double, TYPEDEFD_TYPE>::value                && \
                                                               !std::is_same<long double, TYPEDEFD_TYPE>::value           && \
                                                               !std::is_same<bool, TYPEDEFD_TYPE>::value>::type >            \
      {                                                                                                                      \
         static MPI_Datatype type() { return RETURN_MPI_TYPE; }                                                              \
      };                                                                                                                     \

/// Trigger debugging output when messages sent/recvd
//#define MPI_MSG_DEBUG

namespace Gambit
{

   namespace GMPI
   {

      /// Mapping from (basic) C++ types to MPI datatypes
      /// Based on of "get_hdf5_data_type" in hdf5tools.hpp
      /// Base template is left undefined in order to raise 
      /// a compile error if specialisation doesn't exist.
      template<typename T, typename Enable=void>
      struct get_mpi_data_type;

      /// Overload to work with arrays
      template<typename T, size_t SIZE> 
      struct get_mpi_data_type<T[SIZE]> { static MPI_Datatype type() { return get_mpi_data_type<T>::type(); } };

      /// True types
      /// @{
      template<> struct get_mpi_data_type<char>              { static MPI_Datatype type() { return MPI_CHAR;               } };
      template<> struct get_mpi_data_type<short>             { static MPI_Datatype type() { return MPI_SHORT;              } };
      template<> struct get_mpi_data_type<int>               { static MPI_Datatype type() { return MPI_INT;                } };
      template<> struct get_mpi_data_type<long>              { static MPI_Datatype type() { return MPI_LONG;               } };
      template<> struct get_mpi_data_type<long long>         { static MPI_Datatype type() { return MPI_LONG_LONG;          } };
      template<> struct get_mpi_data_type<unsigned char>     { static MPI_Datatype type() { return MPI_UNSIGNED_CHAR;      } };
      template<> struct get_mpi_data_type<unsigned short>    { static MPI_Datatype type() { return MPI_UNSIGNED_SHORT;     } };
      template<> struct get_mpi_data_type<unsigned int>      { static MPI_Datatype type() { return MPI_UNSIGNED;           } };
      template<> struct get_mpi_data_type<unsigned long>     { static MPI_Datatype type() { return MPI_UNSIGNED_LONG;      } };
      template<> struct get_mpi_data_type<unsigned long long>{ static MPI_Datatype type() { return MPI_UNSIGNED_LONG_LONG; } };
      template<> struct get_mpi_data_type<float>             { static MPI_Datatype type() { return MPI_FLOAT;              } };
      template<> struct get_mpi_data_type<double>            { static MPI_Datatype type() { return MPI_DOUBLE;             } };
      template<> struct get_mpi_data_type<long double>       { static MPI_Datatype type() { return MPI_LONG_DOUBLE;        } };
      /// @}

      /// Typedef'd types; enabled only where they differ from the true types, and where the relevant constants have been
      /// defined in the linked MPI implementation.
      /// @{
      #ifdef MPI_INT8_T
        SPECIALISE_MPI_DATA_TYPE_IF_NEEDED(int8_t,   MPI_INT8_T  )
      #endif
      #ifdef MPI_UINT8_T
        SPECIALISE_MPI_DATA_TYPE_IF_NEEDED(uint8_t,  MPI_UINT8_T )
      #endif
      #ifdef MPI_INT16_T
        SPECIALISE_MPI_DATA_TYPE_IF_NEEDED(int16_t,  MPI_INT16_T )
      #endif
      #ifdef MPI_UINT16_T
        SPECIALISE_MPI_DATA_TYPE_IF_NEEDED(uint16_t, MPI_UINT16_T)
      #endif
      #ifdef MPI_INT32_T 
        SPECIALISE_MPI_DATA_TYPE_IF_NEEDED(int32_t,  MPI_INT32_T )
      #endif
      #ifdef MPI_UINT32_T
        SPECIALISE_MPI_DATA_TYPE_IF_NEEDED(uint32_t, MPI_UINT32_T)
      #endif
      #ifdef MPI_INT64_T
        SPECIALISE_MPI_DATA_TYPE_IF_NEEDED(int64_t,  MPI_INT64_T )
      #endif
      #ifdef MPI_UINT64_T
        SPECIALISE_MPI_DATA_TYPE_IF_NEEDED(uint64_t, MPI_UINT64_T)
      #endif
      /// @}

      /// Main "Communicator" class
      class Comm
      {
         public:
            /// Default Constructor; attaches to MPI_COMM_WORLD
            Comm();

            /// Constructor which copies existing communicator into boundcomm
            Comm(const MPI_Comm& comm);
 
            /// Duplicate existing communicator
            /// (NOTE, this is a collective operation on all procceses)
            void dup(const MPI_Comm& comm);
        
            /// Get total number of MPI tasks in this communicator group
            int Get_size() const;

            /// Get "rank" (ID number) of current task in this communicator group
            int Get_rank() const;

            /// Prevent further executation until all members of the bound communicator group enter the call
            void Barrier()
            {
              #ifdef MPI_MSG_DEBUG
              std::cout<<"rank "<<Get_rank()<<": Barrier() called"<<std::endl;
              #endif 

              int errflag;
              errflag = MPI_Barrier(boundcomm);
              if(errflag!=0) {
                 std::ostringstream errmsg;
                 errmsg << "Error performing MPI_Barrier! Received error flag: "<<errflag; 
                 utils_error().raise(LOCAL_INFO, errmsg.str());
              }

              #ifdef MPI_MSG_DEBUG
              std::cout<<"rank "<<Get_rank()<<": Barrier() passed"<<std::endl;
              #endif 
           }

            /// Blocking receive
            ///  void*        buf      - memory address in which to store received message
            ///  int          count    - number of elements in message
            ///  MPI_Datatype datatype - datatype of each message element
            ///  int          source   - rank of sending (receiving?) process
            ///  int          tag      - message tag          
            ///  MPI_status*  status   - struct containing data about the received message
            /// Returns:
            ///  MPI_status - struct containing data about the received message
            void Recv(void *buf /*out*/, int count, MPI_Datatype datatype, 
                                  int source, int tag, 
                                  MPI_Status *in_status=NULL /*out*/)
            {
              #ifdef MPI_MSG_DEBUG
              std::cout<<"rank "<<Get_rank()<<": Recv() called (count="<<count<<", source="<<source<<", tag="<<tag<<")"<<std::endl;
              #endif 
              int errflag;
              errflag = MPI_Recv(buf, count, datatype, source, tag, boundcomm, in_status == NULL ? MPI_STATUS_IGNORE : in_status);                
              if(errflag!=0)
              {
                std::ostringstream errmsg;
                errmsg << "Error performing MPI_Recv! Received error flag: "<<errflag; 
                utils_error().raise(LOCAL_INFO, errmsg.str());
              }
              #ifdef MPI_MSG_DEBUG
              std::cout<<"rank "<<Get_rank()<<": Recv() finished "<<std::endl;
              #endif 
            }

            /// Templated blocking receive to automatically determine types
            template<class T>
            void Recv(T *buf /*out*/, int count, 
                      int source, int tag, 
                      MPI_Status *status=NULL /*out*/)
            {
               static const MPI_Datatype datatype = get_mpi_data_type<T>::type();
               Recv(buf, count, datatype, source, tag, status);
            }

            /// Blocking send
            void Send(void *buf, int count, MPI_Datatype datatype, 
                                  int destination, int tag)
            {
               #ifdef MPI_MSG_DEBUG
               std::cout<<"rank "<<Get_rank()<<": Send() called (count="<<count<<", destination="<<destination<<", tag="<<tag<<")"<<std::endl;
               #endif 
               int errflag; 
               errflag = MPI_Send(buf, count, datatype, destination, tag, boundcomm);
               if(errflag!=0) {
                 std::ostringstream errmsg;
                 errmsg << "Error performing MPI_Send! Received error flag: "<<errflag; 
                 utils_error().raise(LOCAL_INFO, errmsg.str());
               }
               #ifdef MPI_MSG_DEBUG
               std::cout<<"rank "<<Get_rank()<<": Send() finished"<<std::endl;
               #endif 
            }

            /// Templated blocking send
            template<class T>
            void Send(T *buf, int count, 
                      int destination, int tag)
            {
               static const MPI_Datatype datatype = get_mpi_data_type<T>::type();
               Send(buf, count, datatype, destination, tag);
            }

 
            /// Non-blocking send
            void Isend(void *buf, int count, MPI_Datatype datatype, 
                                  int destination, int tag, 
                                  MPI_Request *request /*out*/)
            {
              #ifdef MPI_MSG_DEBUG
              std::cout<<"rank "<<Get_rank()<<": Isend() called (count="<<count<<", destination="<<destination<<", tag="<<tag<<")"<<std::endl;
              #endif 
              int errflag; 
               errflag = MPI_Isend(buf, count, datatype, destination, tag, boundcomm, request);
               if(errflag!=0) {
                 std::ostringstream errmsg;
                 errmsg << "Error performing MPI_Isend! Received error flag: "<<errflag; 
                 utils_error().raise(LOCAL_INFO, errmsg.str());
               }
            }

            /// Templated Non-blocking send
            template<class T>
            void Isend(T *buf, int count, 
                      int destination, int tag, 
                      MPI_Request *request /*out*/)
            {
               static const MPI_Datatype datatype = get_mpi_data_type<T>::type();
               Isend(buf, count, datatype, destination, tag, request);
            }

            /// Blocking wait for e.g. Isend to complete
            //void Wait(MPI_Request *request, MPI_Status *status)
            //{
            //   MPI_Wait(MPI_Request *request, MPI_Status *status)
            // }

            // Probe for messages waiting to be delivered
            bool Iprobe(int source, int tag, MPI_Status* in_status=NULL /*out*/)
            {
              //#ifdef MPI_MSG_DEBUG
              //std::cout<<"rank "<<Get_rank()<<": Iprobe() called (source="<<source<<", tag="<<tag<<")"<<std::endl;
              //#endif 
               int errflag;
               int you_have_mail; // C does not have a bool type...
               MPI_Status def_status;
               MPI_Status* status;
               if(in_status!=NULL) {
                 status = in_status;
               } else {
                 status = &def_status;
               }
               MPI_Iprobe(source, 1, boundcomm, &you_have_mail, status);
               errflag = MPI_Iprobe(source, tag, boundcomm, &you_have_mail, status);
               if(errflag!=0) {
                 std::ostringstream errmsg;
                 errmsg << "Error performing MPI_Iprobe! Received error flag: "<<errflag; 
                 utils_error().raise(LOCAL_INFO, errmsg.str());
               }
               #ifdef MPI_MSG_DEBUG
               if(you_have_mail!=0) {
                     std::cout<<"rank "<<Get_rank()<<": Iprobe: Message waiting from process "<<status->MPI_SOURCE<<std::endl;
               }
               #endif
               return (you_have_mail != 0);
            }

            // Perform an Isend to all other processes
            // (using templated non-blocking send repeatedly)
            template<class T>
            void IsendToAll(T *buf, int count, int tag, 
                      MPI_Request *request /*out*/)
            {
               int rank = Get_rank();
               int size = Get_size();
               for(int i=0; i<size; i++)
               {
                  if(i!=rank) Isend(buf, count, i, tag, request);
               }
            }

            // Force all processes in this group (possibly all processes in
            // the "WORLD"; implementation dependent) to stop executing.
            // Useful for abnormal termination (since if one processes throws
            // an exception then the others can easily get stuck waiting
            // for messages that will never arrive).
            void Abort()
            {
              std::cout << "rank "<<Get_rank()<<": MPI_Abort command received, attempting to terminate all processes..." << std::endl;
              MPI_Abort(boundcomm, 1);
            }

            /// Tells master to wait until all other processes pass this function, with the specified MPI tag
            void masterWaitForAll(int tag);

            /// Inverse of the above. Everyone waits for master to pass this (but not for anyone else)
            void allWaitForMaster(int tag);

            /// An implementation of Barrier that will fall through if synchronisation takes too long
            /// Could modify to take a function pointer to run while waiting.
            /// Supply MPI tag to identify each particular barrier.
            /// Returns 'false' if barrier succeeds, 'true' if barrier times out (i.e. answers the question "did the barrier time out?")
            bool BarrierWithTimeout(const std::chrono::duration<double> timeout, const int tag, std::ostream& errorlog = std::cout);

            /// A generic place to store a tag commonly used by this communicator
            int mytag = 1;

         private:

            // The MPI communicator to which the current object "talks".
            MPI_Comm boundcomm;
      };

      /// Check if MPI_Init has been called (it is an error to call it twice)
      bool Is_initialized(); 
      
      /// Initialise MPI
      void Init();

      /// Nice wrapper for getting the message size from an MPI_status struct.
      /// Provide the type whose MPI_Datatype you want to retrieve as the
      /// template argument.
      template<class T>
      int Get_count(MPI_Status *status) 
      {
         static const MPI_Datatype datatype = get_mpi_data_type<T>::type();
         int msgsize;
         MPI_Get_count(status, datatype, &msgsize);
         if(msgsize<0)
         {
            std::ostringstream errmsg;
            errmsg << "Error performing MPI_Get_count! Message size returned negative (value was "<<msgsize<<")! This can happen if the number of bytes received is not a multiple of the size of the specified MPI_Datatype. In other words you may have specified a type that doesn't match the type of the sent message; please double-check this."; 
            utils_error().raise(LOCAL_INFO, errmsg.str());
         }
         return msgsize;
      }

      /// @{ Helpers for registration of compound datatypes
 
      /// Structure to hold an MPI startup function plus metadata
      class MpiIniFunc {
        private:
          std::string location;
          std::string name;
          void (*func)();
        public:
          MpiIniFunc(std::string l, std::string n, void(*f)())
            : location(l)
            , name(n)
            , func(f)
          {}
          void runme()
          {
            (*func)();
          }
          std::string mylocation(){return location;}
          std::string myname    (){return name;}
      };

      /// Struct for adding functions to the 'mpi_ini_functions' map
      ///
      /// This will add functions to the map when it is constructed. Works
      /// on the same idea as the "ini_code" struct, except it doesn't
      /// cause the functions to be run, just "queues them up" so to speak.
      struct AddMpiIniFunc {
        AddMpiIniFunc(std::string local_info, std::string name, void(*func)());
      };

      /// @}

   }
}


#endif // include guard
#endif // MPI

