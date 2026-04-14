#pragma once

#include "CoreMinimal.h"
#include "TrapDamageActor.h"
#include "ShooterTrapActor.generated.h"

class USceneComponent;
class UPointLightComponent;
class UParticleSystem;
class USoundBase;

UCLASS()
class DUNGEONCROWLER_API AShooterTrapActor : public ATrapDamageActor
{
	GENERATED_BODY()

public:
	AShooterTrapActor();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void Shoot();

	UFUNCTION()
	void StopEyeGlow();

	void StartFiring();
	void StopFiring();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ShooterTrap")
	USceneComponent* FirePoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ShooterTrap")
	UPointLightComponent* EyeLight;

	FTimerHandle FireTimerHandle;
	FTimerHandle GlowTimerHandle;
	float DefaultEyeIntensity = 0.f;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ShooterTrap")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ShooterTrap", meta = (ClampMin = "0.05"))
	float FireInterval = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ShooterTrap", meta = (ClampMin = "0.0"))
	float ProjectileSpeed = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ShooterTrap")
	bool bAutoFire = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ShooterTrap")
	bool bEnableEyeLight = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ShooterTrap", meta = (EditCondition = "bEnableEyeLight"))
	FLinearColor EyeGlowColor = FLinearColor::Red;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ShooterTrap", meta = (EditCondition = "bEnableEyeLight", ClampMin = "0.0"))
	float EyeGlowIntensity = 5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ShooterTrap", meta = (EditCondition = "bEnableEyeLight", ClampMin = "0.0"))
	float GlowDuration = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ShooterTrap")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ShooterTrap")
	USoundBase* FireSound;
};
