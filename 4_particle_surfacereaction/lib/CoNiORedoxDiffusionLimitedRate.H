/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
    Copyright (C) 2018-2019 OpenCFD Ltd.
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::CoNiORedoxDiffusionLimitedRate

Group
    grpLagrangianIntermediateSurfaceReactionSubModels

Description

    Reference:
    Development of a Kinetic Model for the Redox Reactions of
    Co2.4Ni0.6O4 and SiO2/Co2.4Ni0.6O4 Oxides for Thermochemical
    Energy Storage

    T \in (600,800)
	Oxidation:
	        Co_{1-x}Ni_{x}O(s) + 0.5O2 -> Co_{3-x}Ni_{x}O4(s)
		Ea, A, m, n, p
    T > 850
	Reduction
	        Co_{3-x}Ni_{x}O4(s) -> Co_{1-x}Ni_{x}O(s) + 0.5O2 
		Ea, A, m, n, p
		
	\frac{d\alpha}{dt} = A e^{-Ea/RT} f(\alpha)
	alpha = \frac{m_0 - m_t}{m_0 - m_f}
	f(\alpha) = \alpha^m (1-\alpha)^n [-ln(1-\alpha)]^p
	
	ODE solver needs to be employed to solve alpha, such that mass conversion
	due to red-ox can be determined

\*---------------------------------------------------------------------------*/

#ifndef CoNiORedoxDiffusionLimitedRate_H
#define CoNiORedoxDiffusionLimitedRate_H

#include "SurfaceReactionModel.H"
#include "CoNiORedoxODE.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// Forward declarations
template<class CloudType>
class CoNiORedoxDiffusionLimitedRate;


/*---------------------------------------------------------------------------*\
              Class CoNiORedoxDiffusionLimitedRate Declaration
\*---------------------------------------------------------------------------*/

template<class CloudType>
class CoNiORedoxDiffusionLimitedRate
:
    public SurfaceReactionModel<CloudType>
{
    // Private data
        
            //- Oxidation ODE to be solved
            const CoNiORedoxODE odeOxi_;

            //- Oxidation ODE solver
            autoPtr<ODESolver> odeSolverOxi_;

            //- Reduction ODE to be solved
            const CoNiORedoxODE odeRed_;

            //- Reduction ODE solver
            autoPtr<ODESolver> odeSolverRed_;


            //- Model constants
	        struct ReactionConstant
	        {
                scalar Ea_;

                scalar A_;

                scalar m_;

                scalar n_;

                scalar p_;
	        };

            //- Reaction constant for oxidation reaction
            ReactionConstant RCox_;

            //- Reaction constant for reduction reaction
            ReactionConstant RCred_;

	        //- Call solveODE Oxidation
	        Foam::scalar solveODEOxi(const scalar & T, const scalar & t0, const scalar & dt);

	        //- Call solveODE Reduction
	        Foam::scalar solveODERed(const scalar & T, const scalar & t0, const scalar & dt);

            //- Stoichiometry of reaction
            // const scalar Sb_;

            //- Diffusion coefficient of oxidants [m2/s]
            const scalar D_;


        // Addressing

            //- Co1-xNixO positions in global/local lists
            label CoNiOsLocalId_;

            //- Co3-xNixO4 positions in global/local lists
            label Co3NiO4sLocalId_;

            //- O2 position in global list
            label O2GlobalId_;


        // Local copies of thermo properties

            //- Molecular weight of Co1-xNixO [kg/kmol]
            scalar WCoNiOs_;

            //- Molecular weight of Co3-xNixO4 [kg/kmol]
            scalar WCo3NiO4s_;

            //- Molecular weight of O2 [kg/kmol]
            scalar WO2_;

            //- Formation enthalpy for Co1-xNixO [J/kg]
            scalar HcCoNiO_;

            //- Formation enthalpy for Co3-xNixO4 [J/kg]
            scalar HcCo3NiO4_;


public:

    //- Runtime type information
    TypeName("CoNiORedoxDiffusionLimitedRate");


    // Constructors

        //- Construct from dictionary
        CoNiORedoxDiffusionLimitedRate
        (
            const dictionary& dict,
            CloudType& owner
        );

        //- Construct copy
        CoNiORedoxDiffusionLimitedRate
        (
            const CoNiORedoxDiffusionLimitedRate<CloudType>& srm
        );

        //- Construct and return a clone
        virtual autoPtr<SurfaceReactionModel<CloudType>> clone() const
        {
            return autoPtr<SurfaceReactionModel<CloudType>>
            (
                new CoNiORedoxDiffusionLimitedRate<CloudType>(*this)
            );
        }


    //- Destructor
    virtual ~CoNiORedoxDiffusionLimitedRate() = default;


    // Member Functions

        //- Update surface reactions
        virtual scalar calculate
        (
            const scalar dt,
            const scalar Re,
            const scalar nu,
            const label celli,
            const scalar d,
            const scalar T,
            const scalar Tc,
            const scalar pc,
            const scalar rhoc,
            const scalar mass,
            const scalarField& YGas,
            const scalarField& YLiquid,
            const scalarField& YSolid,
            const scalarField& YMixture,
            const scalar N,
            scalarField& dMassGas,
            scalarField& dMassLiquid,
            scalarField& dMassSolid,
            scalarField& dMassSRCarrier
        ) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "CoNiORedoxDiffusionLimitedRate.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
