#include "ShooterTrapActor.h"
#include "Components/SceneComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"

AShooterTrapActor::AShooterTrapActor()
{
	FirePoint = CreateDefaultSubobject<USceneComponent>(TEXT("FirePoint"));
	FirePoint->SetupAttachment(Trigger);

	EyeLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("EyeLight"));
	EyeLight->SetupAttachment(Mesh);
	EyeLight->SetIntensity(0.f);
	EyeLight->SetLightColor(EyeGlowColor);
	EyeLight->bUseInverseSquaredFalloff = false;
	EyeLight->SetAttenuationRadius(300.f);
	EyeLight->SetCastShadows(false);
}

void AShooterTrapActor::BeginPlay()
{
	Super::BeginPlay();

	if (EyeLight)
	{
		DefaultEyeIntensity = EyeLight->Intensity;
		EyeLight->SetIntensity(0.f);
		EyeLight->SetLightColor(EyeGlowColor);
		EyeLight->SetVisibility(bEnableEyeLight);
	}

	if (bAutoFire && FireInterval > 0.f)
	{
		StartFiring();
	}
}

void AShooterTrapActor::StartFiring()
{
	if (!GetWorld() || FireInterval <= 0.f)
	{
		return;
	}

	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AShooterTrapActor::Shoot, FireInterval, true, FireInterval);
}

void AShooterTrapActor::StopFiring()
{
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void AShooterTrapActor::Shoot()
{
	if (!ProjectileClass || !FirePoint)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FTransform FireTransform = FirePoint->GetComponentTransform();
	AActor* SpawnedActor = World->SpawnActor<AActor>(ProjectileClass, FireTransform);

	if (SpawnedActor)
	{
		if (UProjectileMovementComponent* ProjectileMovement = SpawnedActor->FindComponentByClass<UProjectileMovementComponent>())
		{
			ProjectileMovement->Velocity = FireTransform.GetRotation().GetForwardVector() * ProjectileSpeed;
		}
	}

	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(World, MuzzleEffect, FireTransform);
	}

	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, FirePoint->GetComponentLocation());
	}

	if (bEnableEyeLight && EyeLight)
	{
		EyeLight->SetLightColor(EyeGlowColor);
		EyeLight->SetIntensity(EyeGlowIntensity);
		GetWorldTimerManager().ClearTimer(GlowTimerHandle);
		GetWorldTimerManager().SetTimer(GlowTimerHandle, this, &AShooterTrapActor::StopEyeGlow, GlowDuration, false);
	}
}

void AShooterTrapActor::StopEyeGlow()
{
	if (EyeLight)
	{
		EyeLight->SetIntensity(0.f);
	}
}
