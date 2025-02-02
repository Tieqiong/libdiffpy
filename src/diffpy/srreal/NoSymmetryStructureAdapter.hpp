/*****************************************************************************
*
* libdiffpy         by DANSE Diffraction group
*                   Simon J. L. Billinge
*                   (c) 2010 The Trustees of Columbia University
*                   in the City of New York.  All rights reserved.
*
* File coded by:    Pavol Juhas
*
* See AUTHORS.txt for a list of people who contributed.
* See LICENSE_DANSE.txt for license information.
*
******************************************************************************
*
* class NoSymmetryStructureAdapter -- StructureAdapter class that removes
*     any symmetry expansions (rotations or periodic translations) from
*     another StructureAdapter instance.  This can be used to use only
*     the asymmetric unit from any adapter to crystal structure.
*
* class NoSymmetryBondGenerator -- bond generator
*
* nosymmetry -- factory function that creates a NoSymmetryStructureAdapter
*     instance inside StructureAdapterPtr
*
*****************************************************************************/

#ifndef NOSYMMETRYSTRUCTUREADAPTER_HPP_INCLUDED
#define NOSYMMETRYSTRUCTUREADAPTER_HPP_INCLUDED

#include <diffpy/srreal/StructureAdapter.hpp>

#include <diffpy/Export.hpp>

namespace diffpy {
namespace srreal {

class DLL_EXPORT NoSymmetryStructureAdapter : public StructureAdapter
{
    public:

        // constructors
        NoSymmetryStructureAdapter()  { }
        NoSymmetryStructureAdapter(StructureAdapterPtr);

        // methods - overloaded
        virtual StructureAdapterPtr clone() const;
        virtual BaseBondGeneratorPtr createBondGenerator() const;
        virtual int countSites() const;
        virtual double numberDensity() const;
        virtual const std::string& siteAtomType(int idx) const;
        virtual const R3::Vector& siteCartesianPosition(int idx) const;
        // reusing base-class StructureAdapter::siteMultiplicity()
        virtual double siteOccupancy(int idx) const;
        virtual bool siteAnisotropy(int idx) const;
        virtual const R3::Matrix& siteCartesianUij(int idx) const;
        virtual void customPQConfig(PairQuantity* pq) const;
        virtual StructureDifference diff(StructureAdapterConstPtr) const;

        // methods - own
        StructureAdapterPtr getSourceStructure();
        StructureAdapterConstPtr getSourceStructure() const;

    private:

        // data
        StructureAdapterPtr msrcstructure;

        // serialization
        friend class boost::serialization::access;
        template<class Archive>
            void serialize(Archive& ar, const unsigned int version)
        {
            ar & boost::serialization::base_object<StructureAdapter>(*this);
            ar & msrcstructure;
        }

};

// Routines ------------------------------------------------------------------

/// create NoSymmetryStructureAdapter from an existing StructureAdapter
DLL_EXPORT StructureAdapterPtr nosymmetry(StructureAdapterPtr stru);


/// create NoSymmetryStructureAdapter from an adaptable structure object
template <class T>
StructureAdapterPtr nosymmetry(const T& stru)
{
    StructureAdapterPtr bstru = convertToStructureAdapter(stru);
    return nosymmetry(bstru);
}

}   // namespace srreal
}   // namespace diffpy

// Serialization -------------------------------------------------------------

BOOST_CLASS_EXPORT_KEY(diffpy::srreal::NoSymmetryStructureAdapter)

#endif  // NOSYMMETRYSTRUCTUREADAPTER_HPP_INCLUDED
