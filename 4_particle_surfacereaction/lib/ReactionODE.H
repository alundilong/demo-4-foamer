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
    ODE:
	\frac{d\alpha}{dt} = A e^{-Ea/RT} f(\alpha)
	alpha = \frac{m_0 - m_t}{m_0 - m_f}
	f(\alpha) = \alpha^m (1-\alpha)^n [-ln(1-\alpha)]^p

\*---------------------------------------------------------------------------*/

#ifndef REACTIONODE_H
#define REACTIONODE_H

#include "ODESystem.H"
#include "ODESolver.H"

using namespace Foam;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

class ReactionODE
:
    public ODESystem
{

public:

    ReactionODE(const dictionary& dict);

    label nEqns() const;

    label nSteps() const;
   
    scalar tolerance() const;

    scalar A() const;
   
    scalar Ea() const;

    scalar m() const;

    scalar n() const;

    scalar p() const;

    scalar T() const;

    void setTemperature(const scalar T);

    void derivatives
    (
        const scalar x,
        const scalarField& y,
        scalarField& dydx
    ) const;

    void jacobian
    (
        const scalar x,
        const scalarField& y,
        scalarField& dfdx,
        scalarSquareMatrix& dfdy
    ) const;

private:
    scalar A_;
    scalar Ea_;
    scalar m_;
    scalar n_;
    scalar p_;
    scalar T_;
    label nSteps_;
    scalar tolerance_;
};

#endif
