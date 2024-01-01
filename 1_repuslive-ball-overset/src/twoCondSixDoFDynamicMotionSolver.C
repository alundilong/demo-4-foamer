/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
    Copyright (C) 2018-2022 OpenCFD Ltd.
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

#include "twoCondSixDoFDynamicMotionSolver.H"
#include "addToRunTimeSelectionTable.H"
#include "transformField.H"
#include "cellZoneMesh.H"
#include "bitSet.H"
#include "syncTools.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(twoCondSixDoFDynamicMotionSolver, 0);
    addToRunTimeSelectionTable
    (
        motionSolver,
        twoCondSixDoFDynamicMotionSolver,
        dictionary
    );
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::twoCondSixDoFDynamicMotionSolver::twoCondSixDoFDynamicMotionSolver
(
    const polyMesh& mesh,
    const IOdictionary& dict
)
:
    points0MotionSolver(mesh, dict, typeName)
{
    if(coeffDict().size() != 2) {
    	FatalErrorInFunction
	    << "Has to have 2 body. "
	    << "Current size is. " << coeffDict().size()
	    << abort(FatalError);
    }

    SBMFs_.resize(2);
    pointIDs_.resize(2);

    label zonei = 0;

    bitSet movePts;

    for (const entry& dEntry : coeffDict())
    {
        if (dEntry.isDict())
        {
            const keyType& cellZoneName = dEntry.keyword();

            const dictionary& subDict = dEntry.dict();

            // Also handles groups, multiple zones (as wordRe match) ...
            labelList zoneIDs = mesh.cellZones().indices(cellZoneName);

            if (zoneIDs.empty())
            {
                FatalIOErrorInFunction(coeffDict())
                    << "No matching cellZones: " << cellZoneName << nl
                    << "    Valid zones : "
                    << flatOutput(mesh.cellZones().names()) << nl
                    << "    Valid groups: "
                    << flatOutput(mesh.cellZones().groupNames())
                    << nl
                    << exit(FatalIOError);
            }

            SBMFs_.set
            (
                zonei,
                //solidBodyMotionFunction::New(subDict, mesh.time())
                new Foam::solidBodyMotionFunctions::condSixDoFDynamicMotion(subDict, mesh.time())
            );


            // Markup points associated with cell zone(s)

            movePts.reset();
            movePts.resize(mesh.nPoints());

            for (const label zoneID : zoneIDs)
            {
                for (const label celli : mesh.cellZones()[zoneID])
                {
                    for (const label facei : mesh.cells()[celli])
                    {
                        movePts.set(mesh.faces()[facei]);
                    }
                }
            }

            syncTools::syncPointList
            (
                mesh, movePts, orEqOp<unsigned int>(), 0u
            );

            pointIDs_[zonei] = movePts.sortedToc();

            Info<< "Applying solid body motion " << SBMFs_[zonei].type()
                << " to "
                << returnReduce(pointIDs_[zonei].size(), sumOp<label>())
                << " points of cellZone " << cellZoneName << endl;

            ++zonei;
        }
    }

    SBMFs_.resize(zonei);
    pointIDs_.resize(zonei);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::tmp<Foam::pointField> Foam::twoCondSixDoFDynamicMotionSolver::curPoints() const
{
    auto ttransformedPts = tmp<pointField>::New(mesh().points());
    auto& transformedPts = ttransformedPts.ref();

    //- first body
    vector com0 = SBMFs_[0].CoM();
    vector com1 = SBMFs_[1].CoM();

    vector nij = com0-com1;
    SBMFs_[0].setCCVector(nij);
    SBMFs_[1].setCCVector(-nij);

    forAll(SBMFs_, zonei)
    {
        const labelList& zonePoints = pointIDs_[zonei];

        UIndirectList<point>(transformedPts, zonePoints) = transformPoints
        (
            SBMFs_[zonei].transformation(),
            pointField(points0_, zonePoints)
        );
    }

    return ttransformedPts;
}


// ************************************************************************* //
