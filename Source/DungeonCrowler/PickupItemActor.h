#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupItemActor.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class USoundBase;

UCLASS()
class DUNGEONCROWLER_API APickupItemActor : public AActor
{
	GENERATED_BODY()

public:
	APickupItemActor();

protected:
	virtual void BeginPlay() override;

	// Trigger heredado en el BP
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	UBoxComponent* Trigger;

	// Mesh visible del objeto
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	UStaticMeshComponent* Mesh;

	// Sonido al recoger
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	USoundBase* PickupSound = nullptr;

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