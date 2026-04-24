#include "FireTrapActor.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "MyPlayerCharacter.h"

AFireTrapActor::AFireTrapActor()
{
    PrimaryActorTick.bCanEverTick = false;

    TrapMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrapMesh"));
    SetRootComponent(TrapMesh);
    TrapMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    FireSpawnPoint = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FireSpawnPoint"));
    FireSpawnPoint->SetupAttachment(TrapMesh);
    FireSpawnPoint->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    FireSpawnPoint->SetRelativeLocation(FVector(100.f, 0.f, 0.f));

    ActivationTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("ActivationTrigger"));
    ActivationTrigger->SetupAttachment(TrapMesh);
    ActivationTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    ActivationTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
    ActivationTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    ActivationTrigger->SetBoxExtent(FVector(400.f, 400.f, 150.f));
}

void AFireTrapActor::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    UpdateTriggerSize();
}

void AFireTrapActor::UpdateTriggerSize()
{
    if (ActivationTrigger)
    {
        ActivationTrigger->SetBoxExtent(ActivationTriggerSize);
    }

    if (FireSpawnPoint)
    {
        FireSpawnPoint->SetRelativeLocation(FireSpawnOffset);
    }
}

void AFireTrapActor::BeginPlay()
{
    Super::BeginPlay();

    UpdateTriggerSize();

    ActivationTrigger->OnComponentBeginOverlap.AddDynamic(this, &AFireTrapActor::OnActivationBeginOverlap);
    ActivationTrigger->OnComponentEndOverlap.AddDynamic(this, &AFireTrapActor::OnActivationEndOverlap);
}

void AFireTrapActor::StartFiring()
{
    if (!FireProjectileClass) return;

    GetWorldTimerManager().SetTimer(
        FireTimerHandle,
        this,
        &AFireTrapActor::SpawnFireProjectile,
        FireInterval,
        true,
        0.f
    );
}

void AFireTrapActor::StopFiring()
{
    GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void AFireTrapActor::SpawnFireProjectile()
{
    if (!FireProjectileClass || !GetWorld()) return;

    const FVector SpawnLocation = FireSpawnPoint->GetComponentLocation();

    FRotator FireRotation;
    if (bUseCustomDirection)
    {
        FireRotation = CustomFireDirection;
    }
    else
    {
        FireRotation = GetActorRotation();
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AActor* Projectile = GetWorld()->SpawnActor<AActor>(
        FireProjectileClass,
        SpawnLocation,
        FireRotation,
        SpawnParams
    );

    if (Projectile)
    {
        // Guardar el daño en el proyectil si tiene la variable expuesta
        Projectile->Tags.Add(FName(*FString::Printf(TEXT("FireDamage_%.0f"), FireDamage)));
    }
}

void AFireTrapActor::OnActivationBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || !Cast<ACharacter>(OtherActor)) return;

    bPlayerInRange = true;
    StartFiring();
}

void AFireTrapActor::OnActivationEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (!OtherActor || !Cast<ACharacter>(OtherActor)) return;

    bPlayerInRange = false;
    StopFiring();
}