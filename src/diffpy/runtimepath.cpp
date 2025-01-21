/*****************************************************************************
*
* libdiffpy         Complex Modeling Initiative
*                   (c) 2013 Brookhaven Science Associates,
*                   Brookhaven National Laboratory.
*                   All rights reserved.
*
* File coded by:    Pavol Juhas
*
* See AUTHORS.txt for a list of people who contributed.
* See LICENSE.txt for license information.
*
******************************************************************************
*
* Functions for resolving paths to static data files at runtime.
*
*****************************************************************************/

#include <cstdlib>
#include <climits>
#include <cstring>
#include <cassert>
#include <sstream>
#include <stdexcept>
#include <sys/stat.h>
#include <dlfcn.h> // not available on Windows, conda install dlfcn-win32

// libgen.h is not available on Windows
#ifdef _WIN32
#include <string>
#include <algorithm>

// A simple replacement for dirname() on Windows.
inline std::string win_dirname(const std::string& path)
{
    // Find the last occurrence of either '\' or '/'
    size_t pos = path.find_last_of("\\/");
    if (pos == std::string::npos)
        return ".";  // No directory found, return current directory
    return path.substr(0, pos);
}

// Overload to match the expected C-style interface if needed:
inline char* dirname(char* path)
{
    static std::string dir;
    dir = win_dirname(std::string(path));
    return const_cast<char*>(dir.c_str());
}
#else
#include <libgen.h>
#endif

// S_ISDIR, PATH_MAX, realpath, etc.
#ifdef _WIN32
#include <windows.h>
#include <stdlib.h>    // for _fullpath
#include <sys/stat.h>  // for _stat
#include <direct.h>    // for _getcwd, etc.

// Define S_ISDIR using the Windows _S_IFDIR flag from sys/stat.h.
#ifndef S_ISDIR
#define S_ISDIR(mode) (((mode) & _S_IFDIR) == _S_IFDIR)
#endif

// Use MAX_PATH from <windows.h> as PATH_MAX if not defined.
#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif

// Define realpath in terms of _fullpath.
// _fullpath returns a pointer to the resolved path in the buffer you provide.
// The usage should be nearly equivalent.
#define realpath(N, R) _fullpath((R), (N), PATH_MAX)
#endif


#include <diffpy/version.hpp>
#include <diffpy/runtimepath.hpp>
#define STRINGIFY(m) STRINGIFY_BRAIN_DAMAGE(m)
#define STRINGIFY_BRAIN_DAMAGE(m) #m

using namespace std;

// Helper Functions ----------------------------------------------------------

namespace {

const char* runtimerelpath =
#ifdef DIFFPYRUNTIMERELPATH
    STRINGIFY(DIFFPYRUNTIMERELPATH)
#else
    "../share/diffpy/libdiffpy-"
    STRINGIFY(DIFFPY_VERSION_MAJOR)
    "."
    STRINGIFY(DIFFPY_VERSION_MINOR)
#endif
;

bool isdir(const string& d)
{
    struct stat sd;
    return (stat(d.c_str(), &sd) == 0 && S_ISDIR(sd.st_mode));
}


/// throw runtime error if directory does not exist.
void ensureIsDir(const std::string& d)
{
    if (isdir(d))  return;
    string emsg("Directory '@D' does not exist.");
    emsg = emsg.replace(emsg.find_first_of('@'), 2, d);
    throw runtime_error(emsg);
}


const string& diffpyruntime()
{
    char fpb[PATH_MAX];
    static string librt;
    static bool did_librt = false;
    static string envrt;
    static bool did_envrt = false;
    // check the DIFFPYRUNTIME environment variable.
    char* pe = getenv("DIFFPYRUNTIME");
    if (pe && *pe != '\0')
    {
        static string dprt;
        did_envrt = did_envrt && (dprt == pe);
        if (!did_envrt)
        {
            dprt = pe;
            envrt = pe;
            size_t pt = envrt.find_last_not_of('/');
            if (pt == string::npos)  envrt = envrt.substr(0, 1);
            else  envrt.erase(pt + 1);
            ensureIsDir(envrt);
            envrt = realpath(envrt.c_str(), fpb);
            did_envrt = true;
        }
        return envrt;
    }
    if (did_librt)  return librt;
    // here we need to resolve path of the libdiffpy shared library
    Dl_info i;
    dladdr(reinterpret_cast<void*>(diffpyruntime), &i);
    // first candidate is resolved in relative data path
    strncpy(fpb, i.dli_fname, PATH_MAX);
    string d1 = string(dirname(fpb)) + "/" + runtimerelpath;
    if (isdir(d1))
    {
        librt = realpath(d1.c_str(), fpb);
        did_librt = true;
        return librt;
    }
    // second candidate is resolved with respect to physical library location
    // according to the source tree layout
    string d2 = dirname(realpath(i.dli_fname, fpb));
    d2 += "/../../src/runtime";
    if (isdir(d2))
    {
        librt = realpath(d2.c_str(), fpb);
        did_librt = true;
        return librt;
    }
    // nothing worked - throw exception about the first candidate path.
    ensureIsDir(d1);
    return librt;
}

}   // namespace

// Implementation ------------------------------------------------------------

namespace diffpy {
namespace runtimepath {

string datapath(const std::string& f)
{
    string rv = diffpyruntime();
    rv += (f.empty() ? "" : "/") + f;
    return rv;
}

// class LineReader ----------------------------------------------------------

// Constructor

LineReader::LineReader() :
    lineno(0),
    commentmark("#")
{ }

// Methods

bool LineReader::isignored() const
{
    return words.empty() || this->iscomment();
}


bool LineReader::iscomment() const
{
    bool rv = !commentmark.empty() && !words.empty() &&
            (0 == words[0].compare(0, commentmark.size(), commentmark));
    return rv;
}


bool LineReader::isblank() const
{
    return words.empty();
}


size_t LineReader::wcount() const
{
    return words.size();
}


runtime_error LineReader::format_error(
        const string& filename, const string& edetail)
{
    ostringstream emsg;
    emsg << "Invalid data format in " <<
        filename << " line " << this->lineno << '.';
    if (!edetail.empty())  emsg << '\n' << edetail;
    return runtime_error(emsg.str());
}

// Non-member operators

istream& operator>> (istream& fp, LineReader& line)
{
    getline(fp, line.line);
    if (fp)  line.lineno++;
    istringstream fpline(line.line);
    line.words.clear();
    string w;
    if (line.separator.empty()) {
        while (fpline >> w)  line.words.push_back(w);
    }
    else {
        assert(line.separator.size() == 1);
        const char& sep = line.separator[0];
        while (getline(fpline, w, sep))  line.words.push_back(w);
    }
    return fp;
}

}   // namespace runtimepath
}   // namespace diffpy
