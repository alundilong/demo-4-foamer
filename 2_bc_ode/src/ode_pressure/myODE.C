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

#include "myODE.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //


MyODE::MyODE(const dictionary& dict)
    :
    R_(readScalar(dict.lookup("R"))),
    C_(readScalar(dict.lookup("C"))),
    nSteps_(dict.lookupOrDefault<label>("nSteps",100)),
    tolerance_(dict.lookupOrDefault<scalar>("tolerance",1e-4))
{
}

label MyODE::nEqns() const
{
    return 1;
}

label MyODE::nSteps() const
{
    return nSteps_;
}

scalar MyODE::tolerance() const
{
    return tolerance_;
}

scalar MyODE::R() const
{
    return R_;
}

scalar MyODE::C() const
{
    return C_;
}

void MyODE::derivatives
    (
        const scalar x,
        const scalarField& y,
        const label li,
        scalarField& dydx
    ) const
{
    dydx[0] = -1.0/R_/C_*y[0];
}

void MyODE::jacobian
    (
        const scalar x,
        const scalarField& y,
        const label li,
        scalarField& dfdx,
        scalarSquareMatrix& dfdy
    ) const
{
    dfdx[0] = 0.0; // f derivative to time is 0
    dfdy(0,0) = -1.0/R_/C_; 
}

