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

\*---------------------------------------------------------------------------*/

#include "CoNiORedoxDiffusionLimitedRate.H"
#include "mathematicalConstants.H"
#include "ReactionODE.H"

using namespace Foam::constant;

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class CloudType>
Foam::CoNiORedoxDiffusionLimitedRate<CloudType>::CoNiORedoxDiffusionLimitedRate
(
    const dictionary& dict,
    CloudType& owner
)
:
    SurfaceReactionModel<CloudType>(dict, owner, typeName),
    odeOxi_(ReactionODE(dict.subDict("OxidationODECoeff"))),
    odeSolverOxi_(ODESolver::New(odeOxi_, dict.subDict("OxidationODECoeff"))),
    odeRed_(ReactionODE(dict.subDict("ReductionODECoeff"))),
    odeSolverRed_(ODESolver::New(odeOxi_, dict.subDict("ReductionODECoeff"))),
    D_(this->coeffDict().getScalar("D")),
    CoNiOsLocalId_ (-1),
    Co3NiO4sLocalId_(-1),
    O2GlobalId_(owner.composition().carrierId("O2")),
    WCoNiOs_(0.0),
    WCo3NiO4s_(0.0),
    WO2_(0.0),
    HcCoNiO_(0.0),
    HcCo3NiO4_(0.0)
{
    RCox_.A_ = odeOxi_.A();
    RCox_.Ea_ = odeOxi_.Ea();
    RCox_.m_ = odeOxi_.m();
    RCox_.n_ = odeOxi_.n();
    RCox_.p_= odeOxi_.p();
    RCred_.A_= odeRed_.A();
    RCred_.Ea_ = odeRed_.Ea();
    RCred_.m_ = odeRed_.m();
    RCred_.n_ = odeRed_.n();
    RCred_.p_ = odeRed_.p();
    // Determine CoNiO ids
    label idSolid = owner.composition().idSolid();
    CoNiOsLocalId_ = owner.composition().localId(idSolid, "CoNiO");
    Co3NiO4sLocalId_ = owner.composition().localId(idSolid, "Co3NiO4");

    // Set local copies of thermo properties
    WO2_ = owner.thermo().carrier().W(O2GlobalId_);

    HcCoNiO_ = owner.thermo().carrier().Hc(CoNiOsLocalId_);
    HcCo3NiO4_ = owner.thermo().carrier().Hc(Co3NiO4sLocalId_);

    const scalar YCoNiOloc = owner.composition().Y0(idSolid)[CoNiOsLocalId_];
    const scalar YCo3NiO4loc = owner.composition().Y0(idSolid)[Co3NiO4sLocalId_];
    const scalar YSolidTot = owner.composition().YMixture0()[idSolid];
    Info<< "    CoNiO(s): particle mass fraction = " << YCoNiOloc*YSolidTot << endl;
    Info<< "    Co3NiO4(s): particle mass fraction = " << YCo3NiO4loc*YSolidTot << endl;
}


template<class CloudType>
Foam::CoNiORedoxDiffusionLimitedRate<CloudType>::CoNiORedoxDiffusionLimitedRate
(
    const CoNiORedoxDiffusionLimitedRate<CloudType>& srm
)
:
    SurfaceReactionModel<CloudType>(srm),
    odeOxi_(srm.odeOxi_),
    odeSolverOxi_(srm.odeSolverOxi_),
    odeRed_(srm.odeRed_),
    odeSolverRed_(srm.odeSolverRed_),
    D_(srm.D_),
    CoNiOsLocalId_ (-1),
    Co3NiO4sLocalId_(-1),
    O2GlobalId_(srm.O2GlobalId_),
    WCoNiOs_(srm.WCoNiOs_),
    WCo3NiO4s_(srm.WCo3NiO4s_),
    WO2_(srm.WO2_),
    HcCoNiO_(srm.HcCoNiO_),
    HcCo3NiO4_(srm.HcCo3NiO4_)
{
    RCox_.A_ = odeOxi_.A();
    RCox_.Ea_ = odeOxi_.Ea();
    RCox_.m_ = odeOxi_.m();
    RCox_.n_ = odeOxi_.n();
    RCox_.p_= odeOxi_.p();
    RCred_.A_= odeRed_.A();
    RCred_.Ea_ = odeRed_.Ea();
    RCred_.m_ = odeRed_.m();
    RCred_.n_ = odeRed_.n();
    RCred_.p_ = odeRed_.p();
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class CloudType>
Foam::scalar Foam::CoNiORedoxDiffusionLimitedRate<CloudType>::calculate
(
    const scalar dt,
    const scalar Re,
    const scalar nu,
    const label celli,
    const scalar d,    // parcel diameter
    const scalar T,    // parcel temperature
    const scalar Tc,   // continuous phase temperature
    const scalar pc,   // continuous phase pressure
    const scalar rhoc, // continuous phase density
    const scalar mass,
    const scalarField& YGas,
    const scalarField& YLiquid,
    const scalarField& YSolid,
    const scalarField& YMixture, // parcel mixture (a parcel could a a mixture of liquid,gas,solid)
    const scalar N,
    scalarField& dMassGas,
    scalarField& dMassLiquid,
    scalarField& dMassSolid,
    scalarField& dMassSRCarrier
) const
{
    scalar heatChange = 0.0;
    if (T < 800) {
        // Fraction of remaining combustible material
        const label idSolid = CloudType::parcelType::SLD;
        // calculate the fraction of oxidatable solid
        // within solid portion 
        const scalar fComb = YMixture[idSolid]*YSolid[CoNiOsLocalId_];

        // Surface combustion active combustible fraction is consumed
        if (fComb < SMALL)
        {
            return 0.0;
        }

        const SLGThermo& thermo = this->owner().thermo();

        // Local mass fraction of O2 in the carrier phase
        const scalar YO2 = thermo.carrier().Y(O2GlobalId_)[celli];

        // Change in C mass [kg]
        scalar Sb = 0.5;
        scalar dmCoNiO = 4.0*mathematical::pi*d*D_*YO2*Tc*rhoc/(Sb*(T + Tc))*dt;

        // Limit mass transfer by availability of CoNiO
        dmCoNiO = min(mass*fComb, dmCoNiO);

        // Change in O2 mass [kg]
        const scalar dmO2 = dmCoNiO/WCoNiOs_*Sb*WO2_;

        // Mass of newly created Co3NiO4 [kg]
        const scalar dmCo3NiO4 = dmCoNiO + dmO2;

        // Update local particle CoNiO and Co3NiO4 mass
        dMassSolid[CoNiOsLocalId_] -= dmCoNiO;
        dMassSolid[Co3NiO4sLocalId_] += dmCo3NiO4;

        // Update carrier O2 mass
        dMassSRCarrier[O2GlobalId_] -= dmO2;

        const scalar HsCoNiOs = thermo.solids().properties()[CoNiOsLocalId_].Hs(T);
        const scalar HsCo3NiO4s = thermo.solids().properties()[Co3NiO4sLocalId_].Hs(T);

        // carrier sensible enthalpy exchange handled via change in mass

        // Heat of reaction [J]
        heatChange = dmCoNiO*HsCoNiOs - dmCo3NiO4*HsCo3NiO4s;
    }
    if (T>850)
    {
        // Fraction of remaining combustible material
        const label idSolid = CloudType::parcelType::SLD;
        // calculate the fraction of oxidatable solid
        // within solid portion 
        const scalar fComb = YMixture[idSolid]*YSolid[Co3NiO4sLocalId_];

        // Surface combustion active combustible fraction is consumed
        if (fComb < SMALL)
        {
            return 0.0;
        }

        const SLGThermo& thermo = this->owner().thermo();

        // Change in C mass [kg]
        scalar Sb = 1.0;
        scalar dmCo3NiO4 = 4.0*mathematical::pi*d*Tc*rhoc/(Sb*(T + Tc))*dt;

        // Limit mass transfer by availability of Co3NiO4
        dmCo3NiO4 = min(mass*fComb, dmCo3NiO4);

        // Mass of newly created CoNiO [kg]
        const scalar dmCoNiO = dmCo3NiO4/WCo3NiO4s_*WCoNiOs_;

        // Update local particle CoNiO and Co3NiO4 mass
        dMassSolid[CoNiOsLocalId_] += dmCoNiO;
        dMassSolid[Co3NiO4sLocalId_] -= dmCo3NiO4;

        // Change in O2 mass [kg]
        Sb = 0.5;
        const scalar dmO2 = dmCo3NiO4/WCo3NiO4s_*Sb*WO2_;
        // Update carrier O2 mass
        dMassSRCarrier[O2GlobalId_] += dmO2;

        const scalar HsCoNiOs = thermo.solids().properties()[CoNiOsLocalId_].Hs(T);
        const scalar HsCo3NiO4s = thermo.solids().properties()[Co3NiO4sLocalId_].Hs(T);

        // carrier sensible enthalpy exchange handled via change in mass

        // Heat of reaction [J]
        heatChange = dmCo3NiO4*HsCo3NiO4s - dmCoNiO*HsCoNiOs;
    }

    return heatChange;
}


// ************************************************************************* //
