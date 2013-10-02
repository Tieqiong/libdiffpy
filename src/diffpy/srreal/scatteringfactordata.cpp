/*****************************************************************************
*
* diffpy.srreal     Complex Modeling Initiative
*                   Pavol Juhas
*                   (c) 2013 Brookhaven National Laboratory,
*                   Upton, New York.  All rights reserved.
*
* File coded by:    Pavol Juhas
*
* See AUTHORS.txt for a list of people who contributed.
* See LICENSE.txt for license information.
*
******************************************************************************
*
* X-ray scattering factors for ions and neutral atoms obtained from
* f0_WaasKirf.dat and the associated reference
* D. Waasmaier, A. Kirfel, Acta Cryst. (1995). A51, 416-413
* http://dx.doi.org/10.1107/S0108767394013292
*
* Electron scattering factors approximated from the X-rays
# Reference: International Tables Volume C, page 224.
*
*****************************************************************************/

#include <algorithm>
#include <fstream>
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>
#include <boost/scoped_ptr.hpp>
#include <blitz/array.h>

#include <diffpy/srreal/scatteringfactordata.hpp>
#include <diffpy/srreal/StructureAdapter.hpp>
#include <diffpy/runtimepath.hpp>
#include <diffpy/validators.hpp>

using namespace std;

// Helper classes and functions ----------------------------------------------

namespace {

// X-ray scattering factors

const int WKTerms = 5;

class WaasKirfFormula
{
    public:

        // Constructor
        WaasKirfFormula()
        {
            a = 0.0;
            b = 0.0;
            c = 0.0;
        }

        // methods
        double atstol(double stol) const
        {
            double rv = c + blitz::sum(a * exp(-b * stol * stol));
            return rv;
        }

        // data
        string symbol;
        blitz::TinyVector<double,WKTerms> a;
        blitz::TinyVector<double,WKTerms> b;
        double c;
};


class wksmbl_equal :
    public binary_function<WaasKirfFormula,WaasKirfFormula,bool>
{
    public:
        bool operator() (
                const WaasKirfFormula& wk0, const WaasKirfFormula& wk1) const
        {
            return wk0.symbol == wk1.symbol;
        }
};


size_t hash_value(const WaasKirfFormula& wk)
{
    boost::hash<string> hasher;
    return hasher(wk.symbol);
}


typedef boost::unordered_set<WaasKirfFormula,
        boost::hash<WaasKirfFormula>, wksmbl_equal> SetOfWKFormulas;


const SetOfWKFormulas& getWKFormulasSet()
{
    using namespace diffpy::runtimepath;
    using diffpy::validators::ensureFileOK;
    static boost::scoped_ptr<SetOfWKFormulas> the_set;
    if (the_set)  return *the_set;
    the_set.reset(new SetOfWKFormulas);
    string wkfile = datapath("f0_WaasKirf.dat");
    ifstream fp(wkfile.c_str());
    ensureFileOK(wkfile, fp);
    LineReader line;
    line.commentmark = '#';
    for (WaasKirfFormula wk; fp >> line;)
    {
        if (line.iscomment() && (0 == line.words[0].compare(0, 2, "#S")))
        {
            if (line.wcount() < 3)
            {
                throw line.format_error(wkfile,
                        "Expected at least 3 columns of data.");
            }
            wk.symbol = line.words[2];
            continue;
        }
        if (line.iscomment() && (0 == line.words[0].compare(0, 2, "#L")))
        {
            assert(!wk.symbol.empty());
            fp >> line;
            if (line.wcount() != 11)
            {
                throw line.format_error("Expected 11 values.");
            }
            istringstream fpline(line.line);
            fpline >>
                wk.a[0] >> wk.a[1] >> wk.a[2] >> wk.a[3] >> wk.a[4] >>
                wk.c >> wk.b[0] >> wk.b[1] >> wk.b[2] >> wk.b[3] >> wk.b[4];
            assert(fpline);
            assert(!the_set->count(wk));
            the_set->insert(wk);
            wk.symbol.clear();
        }
    }
    return getWKFormulasSet();
}

// Electron numbers for elements and ions

typedef boost::unordered_map<string,int> ElectronNumberStorage;

ElectronNumberStorage& getElectronNumberTable()
{
    using namespace diffpy::runtimepath;
    using diffpy::validators::ensureFileOK;
    static boost::scoped_ptr<ElectronNumberStorage> entable;
    typedef ElectronNumberStorage::value_type ENPair;
    if (!entable)
    {
        entable.reset(new ElectronNumberStorage);
        string ionfile = datapath("ionlist.dat");
        ifstream fp0(ionfile.c_str());
        ensureFileOK(ionfile, fp0);
        LineReader line;
        while (fp0 >> line)
        {
            if (line.isignored())  continue;
            istringstream fpline(line.line);
            string element;
            int z = 0;
            fpline >> element >> z;
            if (!fpline)
            {
                throw line.format_error(
                        "Expected at least 2 columns for (symbol, Z).");
            }
            entable->insert(ENPair(element, z));
            for (int v; fpline >> v;)
            {
                ostringstream smbl;
                smbl << element << abs(v) << ((v > 0) ? '+' : '-');
                entable->insert(ENPair(smbl.str(), z - v));
            }
        }
        assert(436 <= entable->size());
    }
    return *entable;
}

// Neutron scattering lengths

typedef boost::unordered_map<string,double> NeutronBCStorage;

const NeutronBCStorage& getNeutronBCTable()
{
    using namespace diffpy::runtimepath;
    using diffpy::validators::ensureFileOK;
    typedef NeutronBCStorage::value_type BCPair;
    static boost::scoped_ptr<NeutronBCStorage> bctable;
    if (bctable)  return *bctable;
    bctable.reset(new NeutronBCStorage);
    string nsffile = datapath("nsftable.dat");
    ifstream fp(nsffile.c_str());
    ensureFileOK(nsffile, fp);
    LineReader line;
    line.commentmark = '#';
    line.separator = ',';
    while (fp >> line)
    {
        if (line.isignored())  continue;
        assert(line.wcount() == 11);
        if (line.words[3].empty())  continue;
        string smbl = line.words[0];
        size_t p0 = smbl.find_first_not_of("0123456789-");
        assert(p0 != string::npos);
        smbl.erase(0, p0);
        size_t p1 = smbl.find_last_of('-');
        if (p1 != string::npos)
        {
            smbl = smbl.substr(p1 + 1) + "-" + smbl.substr(0, p1);
        }
        istringstream fpbc(line.words[3]);
        double bc;
        fpbc >> bc;
        assert(fpbc);
        assert(!bctable->count(smbl));
        bctable->insert(BCPair(smbl, bc));
        // elements are not explicitly included if there is just one isotope
        // or if all isotopes are unstable
        size_t p2 = smbl.find_first_of('-');
        if (p2 != string::npos)
        {
            string el = smbl.substr(p2 + 1);
            // there is just one isotope
            const string& chlf = line.words[1];
            bool addel = (chlf == "100") ||
                (!bctable->count(el) && *chlf.rbegin() == 'Y');
            if (addel)
            {
                assert(!bctable->count(el));
                bctable->insert(BCPair(el, bc));
            }
        }
    }
    // define aliases for neutron, deuterium and tritium
    bctable->insert(BCPair("n", bctable->at("1-n")));
    bctable->insert(BCPair("D", bctable->at("2-H")));
    bctable->insert(BCPair("T", bctable->at("3-H")));
    return getNeutronBCTable();
}

}   // namespace

// Implementation ------------------------------------------------------------

namespace diffpy {
namespace srreal {

/// X-ray scattering factor of an element or ion a given Q
double fxrayatq(const string& smbl, double q)
{
    double stol = q / (4 * M_PI);
    return fxrayatstol(smbl, stol);
}


/// X-ray scattering factor of an element or ion a given sin(theta)/lambda
double fxrayatstol(const string& smbl, double stol)
{
    WaasKirfFormula wk;
    wk.symbol = smbl;
    const SetOfWKFormulas& swk = getWKFormulasSet();
    SetOfWKFormulas::const_iterator wkit = swk.find(wk);
    if (wkit == swk.end())
    {
        wk.symbol = atomBareSymbol(smbl);
        int v = atomValence(smbl);
        if (v == 0)  wkit = swk.find(wk);
        if (abs(v) == 1)
        {
            wk.symbol += ((v > 0) ? "1+" : "1-");
            wkit = swk.find(wk);
        }
    }
    // raise exception if no match
    if (wkit == swk.end())
    {
        string emsg("Unknown atom or ion symbol '");
        emsg += smbl + "'.";
        throw invalid_argument(emsg);
    }
    double rv = wkit->atstol(stol);
    return rv;
}


/// Electron scattering factor of an element or ion a given Q
double felectronatq(const string& smbl, double q)
{
    using namespace diffpy::mathutils;
    // resolve Z first so that invalid symbol can throw an exception
    double Z = round(fxrayatstol(smbl, 0.0));
    if (eps_eq(q, 0.0))     return DOUBLE_MAX;
    double stol = q / (4 * M_PI);
    double rv = 0.023934 * (Z - fxrayatstol(smbl, stol)) / (stol * stol);
    return rv;
}


/// Number of electrons for an element or ion
int electronnumber(const string& smbl)
{
    const ElectronNumberStorage& entable = getElectronNumberTable();
    ElectronNumberStorage::const_iterator it;
    it = entable.find(smbl);
    // try to build standard symbol when not found
    if (it == entable.end())
    {
        ostringstream smbl1;
        smbl1 << atomBareSymbol(smbl);
        int v = atomValence(smbl);
        if (v)  smbl1 << abs(v) << ((v > 0) ? '+' : '-');
        it = entable.find(smbl1.str());
    }
    // throw exception if still not found
    if (it == entable.end())
    {
        ostringstream emsg;
        emsg << "Unknown atom symbol '" << smbl << "'.";
        throw invalid_argument(emsg.str());
    }
    return it->second;
}


/// Coherent scattering length of an element or isotope in fm
double bcneutron(const string& smbl)
{
    const NeutronBCStorage& bctable = getNeutronBCTable();
    NeutronBCStorage::const_iterator it = bctable.find(smbl);
    if (it != bctable.end())  return it->second;
    size_t pe = smbl.find_last_not_of("+-012345678 \t");
    assert(pe != string::npos);
    string smblnocharge = smbl.substr(0, pe + 1);
    it = bctable.find(smblnocharge);
    if (it == bctable.end())
    {
        string emsg("Unknown atom or isotope symbol '");
        emsg += smbl + "'.";
        throw invalid_argument(emsg);
    }
    return it->second;
}

}   // namespace srreal
}   // namespace diffpy
