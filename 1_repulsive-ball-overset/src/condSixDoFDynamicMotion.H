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
    Yijin Mao
    University Of Missouri-Columbia


Class
    Foam::solidBodyMotionFunctions::condSixDoFDynamicMotion

Description

SourceFiles
    condSixDoFDynamicMotion.C

\*---------------------------------------------------------------------------*/

#ifndef condSixDoFDynamicMotion_H
#define condSixDoFDynamicMotion_H

#include "solidBodyMotionFunction.H"
#include "primitiveFields.H"
#include "point.H"

#include "sixDoFRigidBodyMotion.H"
#include "uniformDimensionedFields.H"
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace solidBodyMotionFunctions
{

/*---------------------------------------------------------------------------*\
                          Class condSixDoFDynamicMotion Declaration
\*---------------------------------------------------------------------------*/

class condSixDoFDynamicMotion
:
    public solidBodyMotionFunction
{
    // Private data

        // 
        dictionary dict_;

        //- Six DoF motion object
        // mutable becuase transformation() function is const.
        mutable sixDoFRigidBodyMotion motion_;

        //- Current time index (used for updating)
        mutable label curTimeIndex_;

        wordRes patches_;

        // //- Patches to integrate forces
        // const labelHashSet patchSet_;

        //- Reference density required by the forces object for
        //  incompressible calculations, required if rho == rhoInf
        scalar rhoInf_;

        //- Name of density field, optional unless used for an
        //  incompressible simulation, when this needs to be specified
        //  as rhoInf
        word rhoName_;

        //- repulsive force
        scalar repulsiveForce_;

	//- critical distance
	//- when two body is critical distance apart
	//- switch to linear motion
	scalar criticalDist_;
	
	mutable vector nij_;

    // Private Member Functions

        //- No copy construct
        condSixDoFDynamicMotion(const condSixDoFDynamicMotion&) = delete;

        //- No copy assignment
        void operator=(const condSixDoFDynamicMotion&) = delete;


public:

    //- Runtime type information
    TypeName("condSixDoFDynamicMotion");


    // Constructors

        //- Construct from components
        condSixDoFDynamicMotion
        (
            const dictionary& SBMFCoeffs,
            const Time& runTime
        );

        //- Construct and return a clone
        virtual autoPtr<solidBodyMotionFunction> clone() const
        {
            return autoPtr<solidBodyMotionFunction>
            (
                new condSixDoFDynamicMotion
                (
                    SBMFCoeffs_,
                    time_
                )
            );
        }


    //- Destructor
    virtual ~condSixDoFDynamicMotion();


    // Member Functions

	void setCCVector(vector nij) const {
	    nij_ = nij;
	}

	//- access current center of mass
	vector CoM() const;
    	
        //- Return the solid-body motion transformation septernion
        virtual septernion transformation() const;

        //- Update properties from given dictionary
        virtual bool read(const dictionary& SBMFCoeffs);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace solidBodyMotionFunctions
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
