#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupItemActor.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UItemData;

UCLASS()
class DUNGEONCROWLER_API APickupItemActor : public AActor
{
	GENERATED_BODY()

public:
	APickupItemActor();

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Trigger;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, Category = "Pickup")
	TObjectPtr<const UItemData> ItemData;

	UPROPERTY(EditAnywhere, Category = "Pickup", meta = (ClampMin = "1"))
	int32 Quantity = 1;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
};