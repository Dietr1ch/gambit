//   GUM: GAMBIT Universal Models
//   **********************************
///  \file
///
///  Declarations of SARAH class
///
///  **********************************
///
///  \author Sanjay Bloor
///          (sanjay.bloor12@imperial.ac.uk)
///  \date 2017, 2018, 2019
///
///  \author Tomas Gonzalo
///          (tomas.gonzalo@monash.edu)
///  \date 2019 Sep
///
///  ***********************************


#ifndef OPTIONS
#define OPTIONS

#include <iostream>
#include <vector>

class Options
{

    std::string optpackage;
    std::string optmodel;
    std::string optbasemodel;
    std::string optrestriction;
    std::string optLTot;

    public:
        Options(std::string package, std::string model, std::string basemodel, std::string restriction, std::string lagrangian = "LTotal")
        {
            optpackage = package;
            optmodel = model;
            optbasemodel = basemodel;
            optrestriction = restriction;
            optLTot = lagrangian;
        }

        std::string model() { return optmodel; }
        std::string base_model() { return optbasemodel; }
        std::string package() { return optpackage; }
        std::string restriction() { return optrestriction; }
        std::string lagrangian() { return optLTot; }

};


class Outputs
{
    std::string ch;
    std::string mg;
    std::string vev;
    std::string sph;

    public:

        std::string get_ch() { return ch; }
        std::string get_mg() { return mg; }
        std::string get_sph() { return sph; }
        std::string get_vev() { return vev; }

        void set_ch(std::string chdir) { ch = chdir; }
        void set_mg(std::string mgdir) { mg = mgdir; }
        void set_sph(std::string sphdir) { sph = sphdir; }
        void set_vev(std::string vevdir) { vev = vevdir; }
};

void usage(std::string);

Options parse(int, char**);


class Particle
{

    int pdgcode;
    std::string partname;
    int doublespin;
    int chargex3;
    int partcolor;
    std::string alt_partname;
    bool standardmodel;
    std::string partmass;
    std::string alt_partmass;
    bool selfconj;
    std::string antipartname;

    public:

        // Needed for Boost.python interface
        bool operator==(const Particle&) {return false;}
        bool operator!=(const Particle&) {return true;}

        Particle(int pdg, std::string name, int spinX2, int chargeX3, int color, 
                 bool SM, std::string mass, std::string antiname, std::string alt_name = "",
                 std::string alt_mass= "")
        {
            pdgcode = pdg;
            partname = name;
            doublespin = spinX2;
            chargex3 = chargeX3;
            partcolor = color;
            alt_partname = alt_name;
            standardmodel = SM;
            partmass = mass;
            antipartname = antiname;
            alt_partmass = alt_mass;

            if (name == antiname)
            {
                selfconj = true;
            }
            else
            {
                selfconj = false;
            }

        }

        int pdg() { return pdgcode; }
        std::string name() { return partname; }
        bool SM() { return standardmodel; }
        int spinX2() { return doublespin; }
        int chargeX3() { return chargex3; }
        int color() { return partcolor; }
        std::string mass() { return partmass; }
        bool SC() { return selfconj; }
        std::string antiname() { return antipartname; }
        std::string alt_name() { return alt_partname; }
        std::string alt_mass() { return alt_partmass; }

};

class Parameter
{

    std::string paramname;
    std::string blockname;
    int blockindex;
    std::string paramshape;
    std::string alt_paramname;
    bool real;
    bool output;
    std::string boundary_conditions;

 
    public:

        // Needed for Boost.python interface
        bool operator==(const Parameter&) {return false;}
        bool operator!=(const Parameter&) {return true;}

        Parameter(std::string name, std::string block, int index, 
                  std::string alt_name = "", bool is_real = false,
                  std::string shape = "scalar", bool is_output=false,
                  std::string bcs= "")
        {
            paramname = name;
            blockname = block;
            blockindex = index;
            alt_paramname = alt_name;
            boundary_conditions = bcs;
            paramshape = shape;
            output = is_output;
            real = is_real;
        }

        std::string name() { return paramname; }
        std::string block() { return blockname; }
        int index() { return blockindex; }
        std::string alt_name() { return alt_paramname; }
        std::string bcs() { return boundary_conditions; }
        std::string shape() { return paramshape; }
        bool is_output() { return output; }
        bool is_real() { return real; }

        void set_bcs(std::string bc) { boundary_conditions = bc; }
        void set_name(std::string name) { paramname = name; }

};

class Error
{
    bool _is_error;
    std::string _what;

    public:

        // Needed for Boost.python interface
        bool operator==(const Parameter&) {return false;}
        bool operator!=(const Parameter&) {return true;}

        Error()
        {
          _is_error = false;
          _what = "";
        }

        bool is_error() { return _is_error; }
        std::string what() { return _what; }

        void raise(std::string what)
        {
          _is_error = true;
          _what = what;
        }
};

#endif
