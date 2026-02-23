#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemData.h"
#include "WorldPickup.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class YOURGAME_API AWorldPickup : public AActor
{
    GENERATED_BODY()

public:
    AWorldPickup();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
    TObjectPtr<const UItemData> ItemData;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
    int32 Quantity = 1;

protected:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<USphereComponent> Sphere;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UStaticMeshComponent> Mesh;

    UFUNCTION()
    void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Sweep);
};