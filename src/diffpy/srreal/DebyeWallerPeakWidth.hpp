/*****************************************************************************
*
* libdiffpy         by DANSE Diffraction group
*                   Simon J. L. Billinge
*                   (c) 2009 The Trustees of Columbia University
*                   in the City of New York.  All rights reserved.
*
* File coded by:    Christopher Farrow, Pavol Juhas
*
* See AUTHORS.txt for a list of people who contributed.
* See LICENSE_DANSE.txt for license information.
*
******************************************************************************
*
* class DebyeWallerPeakWidth -- peak width calculated assuming independent
*     thermal vibrations of atoms forming a pair.
*
*****************************************************************************/

#ifndef DEBYEWALLERPEAKWIDTH_HPP_INCLUDED
#define DEBYEWALLERPEAKWIDTH_HPP_INCLUDED

#include <diffpy/srreal/PeakWidthModel.hpp>

#include <diffpy/Export.hpp>

namespace diffpy {
namespace srreal {


class DLL_EXPORT DebyeWallerPeakWidth : public PeakWidthModel
{
    public:

        // constructors
        virtual PeakWidthModelPtr create() const;
        virtual PeakWidthModelPtr clone() const;

        // methods
        virtual const std::string& type() const;
        virtual double calculate(const BaseBondGenerator&) const;
        virtual double maxWidth(StructureAdapterPtr,
                double rmin, double rmax) const;

    private:

        // serialization
        friend class boost::serialization::access;

        template<class Archive>
            void serialize(Archive& ar, const unsigned int version)
        {
            using boost::serialization::base_object;
            ar & base_object<PeakWidthModel>(*this);
        }

};

}   // namespace srreal
}   // namespace diffpy

// Serialization -------------------------------------------------------------

BOOST_CLASS_EXPORT_KEY(diffpy::srreal::DebyeWallerPeakWidth)

#endif  // DEBYEWALLERPEAKWIDTH_HPP_INCLUDED
