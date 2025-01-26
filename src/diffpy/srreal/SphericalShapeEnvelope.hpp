/*****************************************************************************
*
* libdiffpy         by DANSE Diffraction group
*                   Simon J. L. Billinge
*                   (c) 2009 The Trustees of Columbia University
*                   in the City of New York.  All rights reserved.
*
* File coded by:    Pavol Juhas
*
* See AUTHORS.txt for a list of people who contributed.
* See LICENSE_DANSE.txt for license information.
*
******************************************************************************
*
* class SphericalShapeEnvelope -- PDF envelope due to spherical shape factor
*
*****************************************************************************/

#ifndef SPHERICALSHAPEENVELOPE_HPP_INCLUDED
#define SPHERICALSHAPEENVELOPE_HPP_INCLUDED

#include <diffpy/srreal/PDFEnvelope.hpp>

#include <diffpy/Export.hpp>

namespace diffpy {
namespace srreal {

/// @class SphericalShapeEnvelope
/// @brief PDF decay due to finite spherical shape of the sample.
/// Spherical shape envelope is not applied for spherical particle
/// diameter, spdiameter, smaller or equal to zero.

class DLL_EXPORT SphericalShapeEnvelope : public PDFEnvelope
{
    public:

        // constructors
        SphericalShapeEnvelope();
        virtual PDFEnvelopePtr create() const;
        virtual PDFEnvelopePtr clone() const;

        // methods
        virtual const std::string& type() const;
        virtual double operator()(const double& r) const;
        void setSPDiameter(double spd);
        const double& getSPDiameter() const;

    private:

        // data
        double mspdiameter;

        // serialization
        friend class boost::serialization::access;

        template<class Archive>
            void serialize(Archive& ar, const unsigned int version)
        {
            using boost::serialization::base_object;
            ar & base_object<PDFEnvelope>(*this);
            ar & mspdiameter;
        }

};  // class SphericalShapeEnvelope

}   // namespace srreal
}   // namespace diffpy

// Serialization -------------------------------------------------------------

BOOST_CLASS_EXPORT_KEY(diffpy::srreal::SphericalShapeEnvelope)

#endif  // SPHERICALSHAPEENVELOPE_HPP_INCLUDED
