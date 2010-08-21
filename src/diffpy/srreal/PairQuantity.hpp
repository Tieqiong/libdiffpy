/*****************************************************************************
*
* diffpy.srreal     by DANSE Diffraction group
*                   Simon J. L. Billinge
*                   (c) 2009 Trustees of the Columbia University
*                   in the City of New York.  All rights reserved.
*
* File coded by:    Pavol Juhas
*
* See AUTHORS.txt for a list of people who contributed.
* See LICENSE.txt for license information.
*
******************************************************************************
*
* class PairQuantity -- general implementation of pair quantity calculator
*
* $Id$
*
*****************************************************************************/

#ifndef PAIRQUANTITY_HPP_INCLUDED
#define PAIRQUANTITY_HPP_INCLUDED

#include <boost/shared_ptr.hpp>
#include <boost/unordered_set.hpp>

#include <diffpy/srreal/PQEvaluator.hpp>
#include <diffpy/srreal/StructureAdapter.hpp>
#include <diffpy/srreal/PairQuantityUtils.hpp>
#include <diffpy/Attributes.hpp>

namespace diffpy {
namespace srreal {

class BaseBondGenerator;

class PairQuantity : public diffpy::Attributes
{
    public:

        // constructor
        PairQuantity();
        virtual ~PairQuantity()  { }

        // methods
        template <class T> const QuantityType& eval(const T&);
        const QuantityType& value() const;
        void mergeParallelValue(const QuantityType&);

        // configuration
        void setStructure(StructureAdapterPtr);
        template <class T> void setStructure(const T&);
        virtual void setRmin(double);
        const double& getRmin() const;
        virtual void setRmax(double);
        const double& getRmax() const;
        void setEvaluator(PQEvaluatorType evtp);
        void setupParallelRun(int cpuindex, int ncpu);
        int countSites() const;
        void maskAllPairs(bool mask);
        void maskSitePair(int i, int j, bool mask);
        bool getPairMask(int i, int j);

    protected:

        friend class PQEvaluatorBasic;
        friend class PQEvaluatorOptimized;

        // methods
        virtual void resizeValue(size_t);
        virtual void resetValue();
        virtual void configureBondGenerator(BaseBondGenerator&) const;
        virtual void addPairContribution(const BaseBondGenerator&, int) { }

        // data
        QuantityType mvalue;
        StructureAdapterPtr mstructure;
        double mrmin;
        double mrmax;
        boost::shared_ptr<PQEvaluatorBasic> mevaluator;
        int mcountsites;
        boost::unordered_set< std::pair<int,int> > minvertpairmask;
        bool mdefaultpairmask;

};

// Template Public Methods ---------------------------------------------------

template <class T>
const QuantityType& PairQuantity::eval(const T& stru)
{
    this->setStructure(stru);
    mevaluator->updateValue(*this);
    return this->value();
}


template <class T>
void PairQuantity::setStructure(const T& stru)
{
    StructureAdapterPtr bstru = createStructureAdapter(stru);
    this->setStructure(bstru);
}


}   // namespace srreal
}   // namespace diffpy

#endif  // PAIRQUANTITY_HPP_INCLUDED
