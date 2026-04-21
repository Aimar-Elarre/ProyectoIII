#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventoryUnlockTrigger.generated.h"

class UBoxComponent;

UCLASS()
class DUNGEONCROWLER_API AInventoryUnlockTrigger : public AActor
{
    GENERATED_BODY()

public:
    AInventoryUnlockTrigger();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trigger")
    TObjectPtr<UBoxComponent> TriggerBox = nullptr;

    UFUNCTION()
    void OnOverlapBegin(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );
};