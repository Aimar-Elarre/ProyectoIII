#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupItemActor.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class DUNGEONCROWLER_API APickupItemActor : public AActor
{
	GENERATED_BODY()
	
public:	
	APickupItemActor();

protected:
	virtual void BeginPlay() override;

	// 🔥 TRIGGER (HEREDADO EN BP)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* Trigger;

	// 🔥 MESH (VISIBLE EN EL MUNDO)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;

	// 🔥 EVENTO DE OVERLAP
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