#ifndef __FACTORY_DEFS_HPP__
#define __FACTORY_DEFS_HPP__

#ifdef __NO_PLUGIN_BOOST__
#include <memory>
#else
#include "boost/shared_ptr.hpp"
#include "boost/enable_shared_from_this.hpp"
#endif

#include <typeinfo>

namespace Gambit
{
        namespace Scanner
        {
#ifdef __NO_PLUGIN_BOOST__
                using std::shared_ptr;
                using std::enable_shared_from_this;
#else
                using boost::shared_ptr;
                using boost::enable_shared_from_this;
#endif
                
                ///Generic function base used my the scanner.  Can be Likelihood, observables, etc.
                template<typename T>
                class Function_Base;
                
                ///Functor that deletes a Function_Base functor
                template<typename T>
                class Function_Deleter;
                                
                ///Generic ptr that takes ownership of a Function_Base.  This is how a plugin will call a function.
                template<typename T>
                class scan_ptr;
                
                template<typename ret, typename... args>
                class Function_Base <ret (args...)> : public enable_shared_from_this<Function_Base <ret (args...)>>
                {
                private:
                        unsigned long long int pointID;
                        friend class Function_Deleter<ret (args...)>;
                        friend class scan_ptr<ret (args...)>;
                        virtual void deleter(Function_Base <ret (args...)> *in) const
                        {
                                delete in;
                        }
                        virtual const std::type_info & type() const {return typeid(ret (args...));}
                        
                public:
                        Function_Base() : pointID(0) {}
                        virtual ret main(const args&...) = 0;
                        virtual ~Function_Base(){} 
                        
                        ret operator () (const args&... params) 
                        {
                                pointID++;
                                return main(params...);
                        }
                        
                        unsigned long long int getPtID() const {return pointID;}
                };
                
                template<typename ret, typename... args>
                class Function_Deleter<ret (args...)>
                {
                private:
                        Function_Base <ret (args...)> *obj;
                        
                public:
                        Function_Deleter(void *in) : obj(static_cast< Function_Base<ret (args...)>* >(in))
                        {
                        }
                        
                        Function_Deleter(const Function_Deleter<ret (args...)> &in) : obj(in.obj) {}
                        
                        void operator =(const Function_Deleter<ret (args...)> &in)
                        {
                                obj = in.obj;
                        }
                        
                        void operator()(Function_Base <ret (args...)> *in)
                        {
                                obj->deleter(in);
                        }
                };
                
                template<typename ret, typename... args>
                class scan_ptr<ret (args...)> : public shared_ptr< Function_Base< ret (args...)> >
                {
                private:
                        typedef shared_ptr< Function_Base< ret (args...) > > s_ptr;
                        
                public:
                        scan_ptr(){}
                        scan_ptr(const scan_ptr &in) : s_ptr (in){}
                        scan_ptr(scan_ptr &&in) : s_ptr (std::move(in)) {}
                        scan_ptr(void *in) : s_ptr
                                (
                                        static_cast< Function_Base<ret (args...)>* >(in), 
                                        Function_Deleter<ret (args...)>(in)
                                ) 
                        {
                                if (typeid(ret (args...)) != this->get()->type())
                                {
                                        scan_err << "scan_ptr and the functor return by \"get functor\" do not have the same type." << scan_end;
                                }
                        }
                        
                        scan_ptr<ret (args...)> &operator=(void *in)
                        {
                                this->s_ptr::operator=
                                (
                                        s_ptr
                                        (
                                                static_cast< Function_Base<ret (args...)>* >(in), 
                                                Function_Deleter<ret (args...)>(in)
                                        )
                                );
                                
                                if (typeid(ret (args...)) != this->get()->type())
                                {
                                        scan_err << "scan_ptr and the functor return by \"get functor\" do not have the same type." << scan_end;
                                }
                        
                                return *this;
                        }
                        
                        scan_ptr<ret (args...)> &operator=(const scan_ptr<ret (args...)> &in)
                        {
                                this->s_ptr::operator=(in);
                        
                                return *this;
                        }
                        
                        scan_ptr<ret (args...)> &operator=(s_ptr &&in)
                        {
                                this->s_ptr::operator=(std::move(in));
                        
                                return *this;
                        }
                        
                        ret operator()(const args&... params)
                        {
                                return (*this)->operator()(params...);
                        }
                };
                
                ///Pure Base class of a plugin Factory function.
                class Factory_Base
                {
                public:
                        virtual void * operator() (const std::string &purpose) const = 0;
                        virtual ~Factory_Base() {};
                };
        }
}

#endif