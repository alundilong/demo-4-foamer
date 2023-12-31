/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2016 OpenFOAM Foundation
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


    Author:
    Ajit Kumar
    Shiv Nadar University

\*---------------------------------------------------------------------------*/

#include "sixDoFDynamicMotion.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "pointPatchDist.H"
#include "pointConstraints.H"
#include "uniformDimensionedFields.H"
#include "forces.H"
#include "mathematicalConstants.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace solidBodyMotionFunctions
{
    defineTypeNameAndDebug(sixDoFDynamicMotion, 0);
    addToRunTimeSelectionTable
    (
        solidBodyMotionFunction,
        sixDoFDynamicMotion,
        dictionary
    );
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::solidBodyMotionFunctions::sixDoFDynamicMotion::sixDoFDynamicMotion
(
    const dictionary& SBMFCoeffs,
    const Time& runTime
)
:
    solidBodyMotionFunction(SBMFCoeffs, runTime),
          dict_(SBMFCoeffs),
          motion_(SBMFCoeffs,SBMFCoeffs,runTime),
          curTimeIndex_(-1),
          patches_(wordRes(SBMFCoeffs.lookup("patches"))),
          rhoInf_(1.0),
          rhoName_(SBMFCoeffs.lookupOrDefault<word>("rho", "rho"))
{
    read(SBMFCoeffs);

//    g_ = SBMFCoeffs.lookupObject<uniformDimensionedVectorField>("g");

}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::solidBodyMotionFunctions::sixDoFDynamicMotion::~sixDoFDynamicMotion()
{}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

Foam::septernion
Foam::solidBodyMotionFunctions::sixDoFDynamicMotion::transformation() const
{
    
    /*
    Ajit: Apr 9
    This class is called by motion solver through this function.
    The aim should be to 
    - integrate force on the solid body patch,
    - calculate the six dof motion 
    - and return the motion as a septernion    
    */

//    Store the motion state at the beginning of the time-step
    bool firstIter = false;
    if (curTimeIndex_ != time_.time().timeIndex())
    {
        motion_.newTime();
        curTimeIndex_ = time_.time().timeIndex();
        firstIter = true;
    }

    // const scalar ramp = min(max((this->db().time().value() - 5)/10, 0), 1);
    const scalar ramp = 1.0;

    // add force
    dictionary forcesDict;
    forcesDict.add("type", functionObjects::forces::typeName);
    forcesDict.add("patches", patches_);

    forcesDict.add("rhoInf", rhoInf_);
    forcesDict.add("rho", rhoName_);
    forcesDict.add("CofR", motion_.centreOfRotation());

    functionObjects::forces f("forces", time_, forcesDict);

    f.calcForcesMoments();

    dimensionedVector g("g", dimAcceleration, Zero);
    if (dict_.found("g"))
    {
        dict_.lookup("g") >> g;
    }

    Info << "g = " << g << endl;

    motion_.update(
        firstIter,
        ramp * (f.forceEff() + motion_.mass() * g.value()),
        ramp * (f.momentEff() + motion_.mass() * (motion_.momentArm() ^ g.value())),
        time_.deltaTValue(),
        time_.deltaT0Value());

    // TODO: extract the motion state after  the update
    // new center of mass
    vector pn = motion_.state().centreOfRotation();

    scalar t = time_.value();
    //const vector displacement = velocity_*t;
    const vector displacement = pn - motion_.initialCentreOfMass();
    quaternion R(1);
    septernion TR(septernion(-displacement)*R);

    DebugInFunction << "Time = " << t << " transformation: " << TR << endl;
    Info << "Time = " << t << "------------------> disp: " << " " << displacement << endl;

    return TR;
}


bool Foam::solidBodyMotionFunctions::sixDoFDynamicMotion::read
(
    const dictionary& SBMFCoeffs
)
{
    solidBodyMotionFunction::read(SBMFCoeffs);

    SBMFCoeffs_.lookup("velocity") >> velocity_;

    return true;
}


// ************************************************************************* //
