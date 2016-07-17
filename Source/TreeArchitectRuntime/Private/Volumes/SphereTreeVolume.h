//$ Copyright 2015 Ali Akbar, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//


#pragma once
#include "TreeVolume.h"
#include "SphereTreeVolume.generated.h"

/**
 *
 */
UCLASS()
class TREEARCHITECTRUNTIME_API ASphereTreeVolume : public ATreeVolume
{
    GENERATED_UCLASS_BODY()
    
public:
    virtual void GeneratePoints(FRandomStream& Random, SpaceNodeList& OutPoints) override;
    
};