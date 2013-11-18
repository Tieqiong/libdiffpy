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
* class CrystalStructureAdapter -- universal adapter for crystal structure
*     composed of asymmetric unit cell and list of symmetry operations in
*     the space group
*
* class CrystalStructureBondGenerator -- bond generator
*
*****************************************************************************/

#ifndef CRYSTALSTRUCTUREADAPTER_HPP_INCLUDED
#define CRYSTALSTRUCTUREADAPTER_HPP_INCLUDED

#include <diffpy/srreal/PeriodicStructureAdapter.hpp>

namespace diffpy {
namespace srreal {

/// store rotation matrix and translation vector for one symmetry operation
class SymOpRotTrans
{
    public:
        R3::Matrix R;
        R3::Vector t;

    private:

        // serialization
        friend class boost::serialization::access;
        template<class Archive>
            void serialize(Archive& ar, const unsigned int version)
        {
            ar & R & t;
        }

};


class CrystalStructureAdapter : public PeriodicStructureAdapter
{
    public:

        // methods - overloaded
        virtual BaseBondGeneratorPtr createBondGenerator() const;
        virtual double numberDensity() const;
        virtual StructureDifference diff(StructureAdapterConstPtr other) const;

        // methods - own
        int countSymOps() const;
        void clearSymOps();
        void addSymOp(const SymOpRotTrans&);
        void addSymOp(const R3::Matrix& R, const R3::Vector& t);
        const SymOpRotTrans& getSymOp(int i);

    private:

        // data
        /// array of symmetry operations
        std::vector<SymOpRotTrans> msymops;
        std::vector<Atom> msymatoms;
        mutable PeriodicStructureAdapter masymunit;
        mutable bool msymmetry_cached;

        // comparison
        friend bool operator==(
                const CrystalStructureAdapter&, const CrystalStructureAdapter&);

        // serialization
        friend class boost::serialization::access;
        template<class Archive>
            void serialize(Archive& ar, const unsigned int version)
        {
            ar & boost::serialization::base_object<PeriodicStructureAdapter>(*this);
            ar & msymops;
            ar & msymatoms;
            ar & masymunit;
            ar & msymmetry_cached;
        }

};


// Comparison functions
bool operator==(const PeriodicStructureAdapter&, const PeriodicStructureAdapter&);
bool operator!=(const PeriodicStructureAdapter&, const PeriodicStructureAdapter&);


class CrystalStructureBondGenerator : public PeriodicStructureBondGenerator
{
    public:

        // constructors
        CrystalStructureBondGenerator(StructureAdapterConstPtr);

        // methods
        // loop control
        virtual void rewind();

        // data access
        virtual const R3::Matrix& Ucartesian1() const;

        // configuration
        virtual void setRmin(double);
        virtual void setRmax(double);

    protected:

        // methods
        virtual bool iterateSymmetry();
        virtual void rewindSymmetry();

        // data
        const CrystalStructureAdapter* mcstructure;

};

}   // namespace srreal
}   // namespace diffpy

#endif  // CRYSTALSTRUCTUREADAPTER_HPP_INCLUDED
