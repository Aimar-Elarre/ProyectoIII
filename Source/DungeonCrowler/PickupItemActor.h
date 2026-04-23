#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupItemActor.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class USoundBase;
class UItemData;
class AMyPlayerCharacter;

UCLASS()
class DUNGEONCROWLER_API APickupItemActor : public AActor
{
    GENERATED_BODY()

public:
    APickupItemActor();

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SetItemData(const UItemData* NewItemData);

    UFUNCTION(BlueprintPure, Category = "Inventory")
    const UItemData* GetItemData() const;

    UFUNCTION(BlueprintCallable, Category = "Pickup")
    void TryPickup(AMyPlayerCharacter* Player);

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
    UBoxComponent* Trigger;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
    UStaticMeshComponent* Mesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
    USoundBase* PickupSound = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
    TObjectPtr<const UItemData> ItemData = nullptr;

    // Oscilación
    UPROPERTY(EditAnywhere, Category = "Animation")
    float OscillationHeight = 20.f;

    UPROPERTY(EditAnywhere, Category = "Animation")
    float OscillationSpeed = 2.f;

private:
    FVector BaseLocation;
    float OscillationTime = 0.f;

    UFUNCTION()
    void OnOverlapBegin(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    UFUNCTION()
    void OnOverlapEnd(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex
    );
};