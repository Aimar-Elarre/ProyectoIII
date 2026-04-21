#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrapDamageActor.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class USoundBase;

UCLASS()
class DUNGEONCROWLER_API ATrapDamageActor : public AActor
{
	GENERATED_BODY()

public:
	ATrapDamageActor();

protected:
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* Trigger;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	USoundBase* TrapSound;

	virtual void BeginPlay() override;

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