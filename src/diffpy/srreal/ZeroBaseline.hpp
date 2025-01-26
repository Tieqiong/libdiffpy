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
* class ZeroBaseline -- linear PDF baseline
*
*****************************************************************************/

#ifndef ZEROBASELINE_HPP_INCLUDED
#define ZEROBASELINE_HPP_INCLUDED

#include <diffpy/srreal/PDFBaseline.hpp>

#include <diffpy/Export.hpp>

namespace diffpy {
namespace srreal {

/// @class ZeroBaseline
/// @brief trivial zero baseline

class DLL_EXPORT ZeroBaseline : public PDFBaseline
{
    public:

        // constructors
        PDFBaselinePtr create() const;
        PDFBaselinePtr clone() const;

        // methods
        const std::string& type() const;
        double operator()(const double& r) const;

    private:

        // serialization
        friend class boost::serialization::access;

        template<class Archive>
            void serialize(Archive& ar, const unsigned int version)
        {
            using boost::serialization::base_object;
            ar & base_object<PDFBaseline>(*this);
        }

};  // class ZeroBaseline

}   // namespace srreal
}   // namespace diffpy

// Serialization -------------------------------------------------------------

BOOST_CLASS_EXPORT_KEY(diffpy::srreal::ZeroBaseline)

#endif  // ZEROBASELINE_HPP_INCLUDED
