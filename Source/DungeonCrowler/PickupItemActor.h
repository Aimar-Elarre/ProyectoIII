#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupItemActor.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class USoundBase;
class UItemData;

UCLASS()
class DUNGEONCROWLER_API APickupItemActor : public AActor
{
    GENERATED_BODY()

public:
    APickupItemActor();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
    UBoxComponent* Trigger;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
    UStaticMeshComponent* Mesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
    USoundBase* PickupSound = nullptr;

    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
    TObjectPtr<UItemData> ItemData = nullptr;

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