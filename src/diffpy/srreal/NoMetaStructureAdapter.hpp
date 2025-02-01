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
* class NoMetaStructureAdapter -- StructureAdapter proxy class that disables
*     customPQConfig of another StructureAdapter instance.  This may be used
*     for preventing the adapter for diffpy.Structure class from applying
*     scale, spdiameter and such metadata to the PDFCalculator.
*
* nometa -- factory function that returns a NoMetaStructureAdapter
*     instance wrapped in StructureAdapterPtr
*
*****************************************************************************/

#ifndef NOMETASTRUCTUREADAPTER_HPP_INCLUDED
#define NOMETASTRUCTUREADAPTER_HPP_INCLUDED

#include <diffpy/srreal/StructureAdapter.hpp>

#include <diffpy/Export.hpp>

namespace diffpy {
namespace srreal {

class DLL_EXPORT NoMetaStructureAdapter : public StructureAdapter
{
    public:

        // constructors
        NoMetaStructureAdapter()  { }
        NoMetaStructureAdapter(StructureAdapterPtr);

        // methods - overloaded
        virtual StructureAdapterPtr clone() const;
        virtual BaseBondGeneratorPtr createBondGenerator() const;
        virtual int countSites() const;
        virtual double numberDensity() const;
        virtual const std::string& siteAtomType(int idx) const;
        virtual const R3::Vector& siteCartesianPosition(int idx) const;
        virtual int siteMultiplicity(int idx) const;
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

/// create NoMetaStructureAdapter from an existing StructureAdapter
DLL_EXPORT StructureAdapterPtr nometa(StructureAdapterPtr stru);


/// create NoMetaStructureAdapter from an adaptable structure object
template <class T>
StructureAdapterPtr nometa(const T& stru)
{
    StructureAdapterPtr bstru = convertToStructureAdapter(stru);
    return nometa(bstru);
}

}   // namespace srreal
}   // namespace diffpy

// Serialization -------------------------------------------------------------

BOOST_CLASS_EXPORT_KEY(diffpy::srreal::NoMetaStructureAdapter)

#endif  // NOMETASTRUCTUREADAPTER_HPP_INCLUDED
