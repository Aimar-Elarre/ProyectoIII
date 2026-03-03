#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrapDamageActor.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class DUNGEONCROWLER_API ATrapDamageActor : public AActor
{
	GENERATED_BODY()

public:
	ATrapDamageActor();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> Trigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trap")
	float Damage = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trap")
	bool bDestroyOnHit = false;

	UFUNCTION()
	void OnTriggerBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
};