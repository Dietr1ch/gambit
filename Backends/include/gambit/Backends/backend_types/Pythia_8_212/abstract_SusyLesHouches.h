#ifndef __abstract_SusyLesHouches_Pythia_8_212_h__
#define __abstract_SusyLesHouches_Pythia_8_212_h__

#include "gambit/Backends/abstractbase.hpp"
#include "forward_decls_abstract_classes.h"
#include "forward_decls_wrapper_classes.h"
#include <string>
#include <istream>
#include "SLHAea/slhaea.h"
#include <map>
#include <vector>
#include <cstddef>

#include "identification.hpp"

// Forward declaration needed by the destructor pattern.
void set_delete_BEptr(CAT_3(BACKENDNAME,_,SAFE_VERSION)::Pythia8::SusyLesHouches*, bool);


// Forward declaration needed by the destructor pattern.
void wrapper_deleter(CAT_3(BACKENDNAME,_,SAFE_VERSION)::Pythia8::SusyLesHouches*);


// Forward declaration for wrapper_creator.
void wrapper_creator(CAT_3(BACKENDNAME,_,SAFE_VERSION)::Pythia8::Abstract_SusyLesHouches*);


namespace CAT_3(BACKENDNAME,_,SAFE_VERSION)
{
    
    
    namespace Pythia8
    {
        class Abstract_SusyLesHouches : public virtual AbstractBase
        {
            public:
    
                virtual int readFile(::std::basic_string<char, std::char_traits<char>, std::allocator<char> >, int, bool) =0;
    
                virtual int readFile__BOSS(::std::basic_string<char, std::char_traits<char>, std::allocator<char> >, int) =0;
    
                virtual int readFile__BOSS(::std::basic_string<char, std::char_traits<char>, std::allocator<char> >) =0;
    
                virtual int readFile__BOSS() =0;
    
                virtual int readFile(::std::basic_istream<char, std::char_traits<char> >&, int, bool) =0;
    
                virtual int readFile__BOSS(::std::basic_istream<char, std::char_traits<char> >&, int) =0;
    
                virtual int readFile__BOSS(::std::basic_istream<char, std::char_traits<char> >&) =0;
    
                virtual int readSLHAea(int, bool) =0;
    
                virtual int readSLHAea__BOSS(int) =0;
    
                virtual int readSLHAea__BOSS() =0;
    
                virtual void setSLHAea(const ::SLHAea::Coll*) =0;
    
                virtual void printHeader() =0;
    
                virtual void printFooter() =0;
    
                virtual void printSpectrum(int) =0;
    
                virtual void printSpectrum__BOSS() =0;
    
                virtual int checkSpectrum() =0;
    
                virtual std::basic_string<char, std::char_traits<char>, std::allocator<char> >& slhaFile_ref__BOSS() =0;
    
                virtual std::map<int, int, std::less<int>, std::allocator<std::pair<const int, int> > >& decayIndices_ref__BOSS() =0;
    
                virtual std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >& qnumbersName_ref__BOSS() =0;
    
                virtual std::vector<std::basic_string<char>, std::allocator<std::basic_string<char> > >& qnumbersAntiName_ref__BOSS() =0;
    
                virtual int verbose() =0;
    
                virtual void verbose(int) =0;
    
                virtual void message(int, ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >, int) =0;
    
                virtual void message__BOSS(int, ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ::std::basic_string<char, std::char_traits<char>, std::allocator<char> >) =0;
    
                virtual void toLower(::std::basic_string<char, std::char_traits<char>, std::allocator<char> >&) =0;
    
            public:
                virtual void pointer_assign__BOSS(Abstract_SusyLesHouches*) =0;
                virtual Abstract_SusyLesHouches* pointer_copy__BOSS() =0;
    
            private:
                SusyLesHouches* wptr;
                bool delete_wrapper;
            public:
                SusyLesHouches* get_wptr() { return wptr; }
                void set_wptr(SusyLesHouches* wptr_in) { wptr = wptr_in; }
                bool get_delete_wrapper() { return delete_wrapper; }
                void set_delete_wrapper(bool del_wrp_in) { delete_wrapper = del_wrp_in; }
    
            public:
                Abstract_SusyLesHouches()
                {
                    wptr = 0;
                    delete_wrapper = false;
                }
    
                Abstract_SusyLesHouches(const Abstract_SusyLesHouches&)
                {
                    wptr = 0;
                    delete_wrapper = false;
                }
    
                Abstract_SusyLesHouches& operator=(const Abstract_SusyLesHouches&) { return *this; }
    
                virtual void init_wrapper()
                {
                    if (wptr == 0)
                    {
                        wrapper_creator(this);
                        delete_wrapper = true;
                    }
                }
    
                SusyLesHouches* get_init_wptr()
                {
                    init_wrapper();
                    return wptr;
                }
    
                SusyLesHouches& get_init_wref()
                {
                    init_wrapper();
                    return *wptr;
                }
    
                virtual ~Abstract_SusyLesHouches()
                {
                    if (wptr != 0)
                    {
                        set_delete_BEptr(wptr, false);
                        if (delete_wrapper == true)
                        {
                            wrapper_deleter(wptr);
                            wptr = 0;
                            delete_wrapper = false;
                        }
                    }
                }
        };
    }
    
}


#include "gambit/Backends/backend_undefs.hpp"


#endif /* __abstract_SusyLesHouches_Pythia_8_212_h__ */
