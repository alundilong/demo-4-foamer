/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2018-2023 OpenFOAM Foundation
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

\*---------------------------------------------------------------------------*/

#include "odePressureFvPatchScalarField.H"
#include "addToRunTimeSelectionTable.H"
#include "fvPatchFieldMapper.H"
#include "volFields.H"
#include "uniformDimensionedFields.H"
#include "IOmanip.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::odePressureFvPatchScalarField::odePressureFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const dictionary& dict
)
:
    fixedValueFvPatchScalarField(p, iF, dict, false),
    p_("p", dict, p.size()),
    pOld_("p", dict, p.size()),
    timeIndex_(-1),
    ode_(MyODE(dict.subDict("ODECoeff"))),
    odeSolver_(ODESolver::New(ode_, dict))
{
    if (dict.found("value"))
    {
        fvPatchScalarField::operator=
        (
            scalarField("value", dict, p.size())
        );
    }
    else
    {
        fvPatchField<scalar>::operator=(p_);
    }
}


Foam::odePressureFvPatchScalarField::odePressureFvPatchScalarField
(
    const odePressureFvPatchScalarField& ptf,
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    fixedValueFvPatchScalarField(ptf, p, iF, mapper),
    p_(mapper(ptf.p_)),
    pOld_(mapper(ptf.pOld_)),
    timeIndex_(ptf.timeIndex_),
    ode_(ptf.ode_),
    odeSolver_(ptf.odeSolver_)
{}


Foam::odePressureFvPatchScalarField::odePressureFvPatchScalarField
(
    const odePressureFvPatchScalarField& ptf,
    const DimensionedField<scalar, volMesh>& iF
)
:
    fixedValueFvPatchScalarField(ptf, iF),
    p_(ptf.p_),
    pOld_(ptf.pOld_),
    timeIndex_(ptf.timeIndex_),
    ode_(ptf.ode_),
    odeSolver_(ptf.odeSolver_)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::odePressureFvPatchScalarField::map
(
    const fvPatchScalarField& ptf,
    const fvPatchFieldMapper& mapper
)
{
    fixedValueFvPatchScalarField::map(ptf, mapper);

    const odePressureFvPatchScalarField& tiptf =
        refCast<const odePressureFvPatchScalarField>(ptf);

    mapper(p_, tiptf.p_);
}


void Foam::odePressureFvPatchScalarField::reset
(
    const fvPatchScalarField& ptf
)
{
    fixedValueFvPatchScalarField::reset(ptf);

    const odePressureFvPatchScalarField& tiptf =
        refCast<const odePressureFvPatchScalarField>(ptf);

    p_.reset(tiptf.p_);
}

Foam::scalar Foam::odePressureFvPatchScalarField::solveODE(const scalar & p0, const scalar & t0, const scalar & dt)
{
    scalar x = t0;
    scalar xEnd = x + dt;
    scalarField y(1, p0);
    const label & nSteps = ode_.nSteps();
    scalar dxEst = dt/nSteps;
    const scalar & tolerance = ode_.tolerance();
    odeSolver_->relTol() = tolerance;

    Info << "Solving ODE " << endl;
    Info << "startValue " << y << endl;
    Info << "startTime " << t0 << endl;
    Info << "endTime " << xEnd << endl;
    Info << "nSteps = " << nSteps << endl;
    Info << "tolerance = " << tolerance << endl;
    odeSolver_->solve(x, xEnd, y, 0, dxEst);
    scalar yana = 1.0*Foam::exp(-xEnd/ode_.R()/ode_.C());
    Info << "solution = " << y[0] << setw(24) << yana << setw(24) << db().time().timeIndex()<< endl;

    return y[0];
}


void Foam::odePressureFvPatchScalarField::updateCoeffs()
{
    if (updated())
    {
        return;
    }
    scalar t0 = this->db().time().value();
    scalar dt = this->db().time().deltaT().value();

    if(db().time().timeIndex() != timeIndex_)
    {
        pOld_ = *this;
	timeIndex_ = db().time().timeIndex();
    }

    p_ = solveODE(average(pOld_), t0-dt, dt);
    operator==(p_);

    fixedValueFvPatchScalarField::updateCoeffs();
}


void Foam::odePressureFvPatchScalarField::write(Ostream& os) const
{
    fvPatchScalarField::write(os);
    writeEntry(os, "p", p_);
    writeEntry(os, "value", *this);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
    makePatchTypeField
    (
        fvPatchScalarField,
        odePressureFvPatchScalarField
    );
}

// ************************************************************************* //
