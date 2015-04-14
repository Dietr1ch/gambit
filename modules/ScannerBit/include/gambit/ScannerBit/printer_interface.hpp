//  GAMBIT: Global and Modular BSM Inference Tool
//  *********************************************
///  \file
///
///  declaration for scanner module
///
///  *********************************************
///
///  Authors (add name and date if you modify):
//
///  \author Gregory Martinez
///          (gregory.david.martinez@gmail.com)
///  \date 2013 August 2013 Feb 2014
///
///  *********************************************

#include "gambit/Printers/baseprintermanager.hpp"
#include "gambit/Printers/basebaseprinter.hpp"

#ifndef PRINTER_INTERFACE_HPP
#define PRINTER_INTERFACE_HPP

#include <string>
#include <vector>

namespace Gambit
{
        namespace Scanner
        {
                //class printer_interface_temp
                //{
                //public:
                //        virtual void inputValue() = 0;
                //        virtual void inputValue(int thread, int tag, const std::string &, double a);
                //        virtual void inputValue(int thread, int tag, const std::string &, const std::vector<double> &);
                //        template<typename T> void inputValue(int, int, const std::string &, T) {/*do nothing*/}
                //};
                
                /// typedef printer_interface_temp printer_interface;
                typedef Printers::BasePrinterManager printer_interface;
                
                /// Type of the printer objects
                typedef Printers::BaseBasePrinter printer;

        }
}

#endif