//$ Copyright 2015 Ali Akbar, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "TreeArchitectRuntimePrivatePCH.h"
#include "SphereTreeVolume.h"

ASphereTreeVolume::ASphereTreeVolume(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void ASphereTreeVolume::GeneratePoints(FRandomStream& Random, SpaceNodeList& OutPoints) {
    FVector BoxOrigin, BoxExtent;
    GetActorBounds(false, BoxOrigin, BoxExtent);
    
    for (int i = 0; i < NumPoints; i++) {
        FVector PointOnSphere = Random.GetUnitVector();
        float Depth = pow(Random.FRand(), 1 / 3.0f);
        PointOnSphere =  BoxOrigin + PointOnSphere * Depth * BoxExtent;
        
        TSharedPtr<SpaceNode> spaceNode = MakeShareable(new SpaceNode(PointOnSphere, nullptr));
        OutPoints.Add(spaceNode);
    }
}