/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2011-2019 OpenFOAM Foundation
     \\/     M anipulation  |
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

Description

\*---------------------------------------------------------------------------*/

#include "CoNiORedoxODE.H"
#include "physicoChemicalConstants.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

CoNiORedoxODE::CoNiORedoxODE(const dictionary& dict)
    :
    A_(readScalar(dict.lookup("A"))),
    Ea_(readScalar(dict.lookup("Ea"))),
    m_(readScalar(dict.lookup("m"))),
    n_(readScalar(dict.lookup("n"))),
    p_(readScalar(dict.lookup("p"))),
    T_(dict.lookupOrDefault<scalar>("T",300)),
    nSteps_(dict.lookupOrDefault<label>("nSteps",100)),
    tolerance_(dict.lookupOrDefault<scalar>("tolerance",1e-4))
{
}

label CoNiORedoxODE::nEqns() const
{
    return 1;
}

label CoNiORedoxODE::nSteps() const
{
    return nSteps_;
}

scalar CoNiORedoxODE::tolerance() const
{
    return tolerance_;
}

scalar CoNiORedoxODE::A() const
{
    return A_;
}

scalar CoNiORedoxODE::Ea() const
{
    return Ea_;
}

scalar CoNiORedoxODE::m() const
{
    return m_;
}

scalar CoNiORedoxODE::n() const
{
    return n_;
}

scalar CoNiORedoxODE::p() const
{
    return p_;
}

scalar CoNiORedoxODE::T() const
{
    return T_;
}

void CoNiORedoxODE::setTemperature(const scalar T) 
{
    T_ = T;
}


void CoNiORedoxODE::derivatives
    (
        const scalar x,
        const scalarField& y,
        scalarField& dydx
    ) const
{
    scalar alpha = y[0];
    scalar f_alpha = Foam::pow(alpha,m_)*Foam::pow((1.0-alpha),n_)*Foam::pow(-Foam::log(1.0-alpha),p_);
    scalar R = Foam::constant::physicoChemical::R.value();
    dydx[0] = A_*Foam::exp(-Ea_/R/T_)*f_alpha;
}

void CoNiORedoxODE::jacobian
    (
        const scalar x,
        const scalarField& y,
        scalarField& dfdx,
        scalarSquareMatrix& dfdy
    ) const
{
    dfdx[0] = 0.0; // f derivative to time is 0
    scalar alpha = y[0];
    scalar term1 = m_*Foam::pow(alpha,m_-1) * Foam::pow(-(Foam::log(1.0-alpha)),p_) * Foam::pow((1.0-alpha),n_);
    scalar term2 = n_*Foam::pow(alpha,m_) * Foam::pow(-(Foam::log(1.0-alpha)),p_) * Foam::pow((1.0-alpha),n_-1);
    scalar term3 = p_*Foam::pow(alpha,m_) * Foam::pow(-(Foam::log(1.0-alpha)),p_) * Foam::pow((1.0-alpha),n_-1)/Foam::log(1-alpha);
    scalar f_prime_alpha = term1 - term2 - term3;
    scalar R = Foam::constant::physicoChemical::R.value();
    dfdy(0,0) = A_*Foam::exp(-Ea_/R/T_)*f_prime_alpha; 
}


