#/bin/bash

rm -rf [0-9].[0-9]* 1 2 3 4 constant/polyMesh 0
cp -r 0_orig 0

blockMesh
topoSet
topoSet -dict system/topoSetDict2
subsetMesh disk -patch hole -overwrite
subsetMesh disk1 -patch hole1 -overwrite
setFields
