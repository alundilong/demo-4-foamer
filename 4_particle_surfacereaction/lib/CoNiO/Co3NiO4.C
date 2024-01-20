/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
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

#include "Co3NiO4.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(Co3NiO4, 0);
    addToRunTimeSelectionTable(solidProperties, Co3NiO4,);
    addToRunTimeSelectionTable(solidProperties, Co3NiO4, dictionary);
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::Co3NiO4::Co3NiO4()
:
    solidProperties(2710, 850, 1.3, 0.0, 1.0, 100.086, 0.0, 0.0)
{
    if (debug)
    {
        WarningInFunction
            << "Properties of Co3NiO4 need to be checked!!!"
            << endl;
    }
}


Foam::Co3NiO4::Co3NiO4(const dictionary& dict)
:
    Co3NiO4()
{
    readIfPresent(dict);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::Co3NiO4::writeData(Ostream& os) const
{
    solidProperties::writeData(os);
}


// * * * * * * * * * * * * * * IOStream operators  * * * * * * * * * * * * * //

Foam::Ostream& Foam::operator<<(Ostream& os, const Co3NiO4& s)
{
    s.writeData(os);
    return os;
}


// ************************************************************************* //
