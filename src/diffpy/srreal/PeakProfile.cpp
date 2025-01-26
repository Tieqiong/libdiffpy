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
* class PeakProfile -- base class for calculation of peak profiles.
*
*****************************************************************************/

#include <diffpy/srreal/PeakProfile.hpp>
#include <diffpy/HasClassRegistry.ipp>
#include <diffpy/serialization.ipp>

namespace diffpy {

// Unique instantiation of the template registry base class.
template class DLL_EXPORT HasClassRegistry<srreal::PeakProfile>;

namespace srreal {

//////////////////////////////////////////////////////////////////////////////
// class PeakProfile
//////////////////////////////////////////////////////////////////////////////

// Constructors --------------------------------------------------------------

PeakProfile::PeakProfile() : mprecision(0.0)
{
    this->registerDoubleAttribute("peakprecision",
            this, &PeakProfile::getPrecision, &PeakProfile::setPrecision);
}

// Public Methods ------------------------------------------------------------

void PeakProfile::setPrecision(double eps)
{
    if (mprecision != eps)  mticker.click();
    mprecision = eps;
}


const double& PeakProfile::getPrecision() const
{
    return mprecision;
}

}   // namespace srreal
}   // namespace diffpy

// Serialization -------------------------------------------------------------

DIFFPY_INSTANTIATE_SERIALIZATION(diffpy::srreal::PeakProfile)
DIFFPY_INSTANTIATE_PTR_SERIALIZATION(diffpy::srreal::PeakProfile)

// End of file
