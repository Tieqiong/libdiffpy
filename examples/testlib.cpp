// Test program for checking the installation of libdiffpy shared library.
// Compile and run this code using:
//
//      c++ testlib.cpp -ldiffpy
//      ./a.out


#include <iostream>
#include <diffpy/srreal/ScatteringFactorTable.hpp>
// #include <diffpy/srreal/ObjCrystStructureAdapter.hpp>
// #include <ObjCryst/ObjCryst/Crystal.h>

int main(int argc, char* argv[])
{
    using namespace diffpy::srreal;
    try
    {
        ScatteringFactorTablePtr ntbl;
        ntbl = ScatteringFactorTable::createByType("neutron");
        ntbl->lookup("Pb");
        // ObjCryst::SpaceGroup sg("P 1");
        std::cout << "Installation of libdiffpy shared library works!\n";
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
