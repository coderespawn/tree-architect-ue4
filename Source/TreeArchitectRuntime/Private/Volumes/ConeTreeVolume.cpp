//$ Copyright 2015 Ali Akbar, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "TreeArchitectRuntimePrivatePCH.h"
#include "ConeTreeVolume.h"

AConeTreeVolume::AConeTreeVolume(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}


void AConeTreeVolume::GeneratePoints(FRandomStream& Random, SpaceNodeList& OutPoints) {
    FVector BoxOrigin, BoxExtent;
    GetActorBounds(false, BoxOrigin, BoxExtent);
    BoxOrigin.Z -= BoxExtent.Z;
    BoxExtent.Z *= 2;
    
    for (int i = 0; i < NumPoints; i++) {
        FVector PointOnCone = Random.VRandCone(FVector(0, 0, 1), PI);
        float Depth = pow(Random.FRand(), 1 / 3.0f);
        PointOnCone =  BoxOrigin + PointOnCone * FVector(Depth, Depth, 1) * BoxExtent;
        
        TSharedPtr<SpaceNode> spaceNode = MakeShareable(new SpaceNode(PointOnCone, nullptr));
        OutPoints.Add(spaceNode);
    }
}
