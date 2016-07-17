//$ Copyright 2015 Ali Akbar, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "TreeArchitectRuntimePrivatePCH.h"
#include "TreeVolume.h"

ATreeVolume::ATreeVolume(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , NumPoints(1000)
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
}

#if WITH_EDITOR
void ATreeVolume::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    RebuildTree();
}

void ATreeVolume::PostEditMove(bool bFinished)
{
    Super::PostEditMove(bFinished);
    RebuildTree();
}
#endif

void ATreeVolume::RebuildTree()
{
    if (RealtimeUpdate && Tree) {
        Tree->Build();
    }
}
