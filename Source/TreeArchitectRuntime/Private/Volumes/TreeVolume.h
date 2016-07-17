//$ Copyright 2015 Ali Akbar, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//
#pragma once
#include "../TreeArchitect.h"
#include "GameFramework/Volume.h"
#include "Algorithm/SpaceColonization/Tree.h"
#include "TreeVolume.generated.h"


UCLASS(abstract)
class TREEARCHITECTRUNTIME_API ATreeVolume : public AVolume
{
    GENERATED_BODY()
    
public:
    ATreeVolume(const FObjectInitializer& ObjectInitializer);
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tree)
    ATreeArchitect* Tree;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tree)
    bool RealtimeUpdate;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tree)
    int32 NumPoints;
    
    virtual bool ShouldTickIfViewportsOnly() const { return true; }
    
    virtual void GeneratePoints(FRandomStream& Random, SpaceNodeList& OutPoints) {}
    
#if WITH_EDITOR
    //Begin UObject Interface
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostEditMove(bool bFinished) override;
    //End UObject Interface
#endif // WITH_EDITOR
    
protected:
    virtual void RebuildTree();
    
};