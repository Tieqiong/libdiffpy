/*****************************************************************************
*
* libdiffpy         by DANSE Diffraction group
*                   Simon J. L. Billinge
*                   (c) 2009 The Trustees of Columbia University
*                   in the City of New York.  All rights reserved.
*
* File coded by:    Chris Farrow
*
* See AUTHORS.txt for a list of people who contributed.
* See LICENSE_DANSE.txt for license information.
*
******************************************************************************
*
* StructureAdapterPtr createStructureAdapter(const ObjCryst::Crystal&)
* StructureAdapterPtr createStructureAdapter(const ObjCryst::Molecule&)
*   -- structure adapter factories for ObjCryst objects.
*
*****************************************************************************/

#ifndef OBJCRYSTSTRUCTUREADAPTER_HPP_INCLUDED
#define OBJCRYSTSTRUCTUREADAPTER_HPP_INCLUDED

#include <diffpy/srreal/AtomicStructureAdapter.hpp>
#include <diffpy/srreal/CrystalStructureAdapter.hpp>
#include <ObjCryst/ObjCryst/Crystal.h>

#include <diffpy/Export.hpp>

#include <ObjCryst/ObjCryst/Molecule.h>

namespace diffpy {
namespace srreal {

// ObjCryst::Crystal is now adapted with CrystalStructureAdapter

DLL_EXPORT StructureAdapterPtr
createStructureAdapter(const ObjCryst::Crystal& cryst);

// ObjCryst::Molecule can be adapted with AtomicStructureAdapter
//
// Molecules are always considered aperiodic. The anisotropic ADPs are treated
// as if in a cartesian cell. If this is not what is intended, pass the
// molecule as a scattering component within an ObjCryst::Crystal.


DLL_EXPORT StructureAdapterPtr
createStructureAdapter(const ObjCryst::Molecule& molecule);

}   // namespace srreal
}   // namespace diffpy

// Required for argument-lookup visibility when instantiating the
// convertToStructureAdapter template.
namespace ObjCryst {
using ::diffpy::srreal::createStructureAdapter;
}

#endif  // OBJCRYSTSTRUCTUREADAPTER_HPP_INCLUDED
