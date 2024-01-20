/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
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
    Foam::Co3NiO4

Description
    Co3NiO4

SourceFiles
    Co3NiO4.C

\*---------------------------------------------------------------------------*/

#ifndef solid_Co3NiO4_H
#define solid_Co3NiO4_H

#include "solidProperties.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                          Class Co3NiO4 Declaration
\*---------------------------------------------------------------------------*/

class Co3NiO4
:
    public solidProperties
{

public:

    //- Runtime type information
    TypeName("Co3NiO4");


    // Constructors

        //- Construct null
        Co3NiO4();

        //- Construct from dictionary
        Co3NiO4(const dictionary& dict);

        //- Construct and return clone
        virtual autoPtr<solidProperties> clone() const
        {
            return autoPtr<solidProperties>::NewFrom<Co3NiO4>(*this);
        }


    // I-O

        //- Write the function coefficients
        void writeData(Ostream& os) const;

        //- Ostream Operator
        friend Ostream& operator<<(Ostream& os, const Co3NiO4& s);
};


Ostream& operator<<(Ostream& os, const Co3NiO4& s);


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
