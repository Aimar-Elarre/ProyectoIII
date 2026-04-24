#include "MovingPlatformActor.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"

AMovingPlatformActor::AMovingPlatformActor()
{
    PrimaryActorTick.bCanEverTick = true;

    PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
    SetRootComponent(PlatformMesh);
    PlatformMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    StandTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("StandTrigger"));
    StandTrigger->SetupAttachment(PlatformMesh);
    StandTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    StandTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
    StandTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    StandTrigger->SetRelativeLocation(FVector(0.f, 0.f, 50.f));

    ProximityTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("ProximityTrigger"));
    ProximityTrigger->SetupAttachment(PlatformMesh);
    ProximityTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    ProximityTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
    ProximityTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AMovingPlatformActor::BeginPlay()
{
    Super::BeginPlay();

    OriginLocation = GetActorLocation();

    FVector Direction = FVector(
        bMoveX ? DirectionX : 0.f,
        bMoveY ? DirectionY : 0.f,
        bMoveZ ? DirectionZ : 0.f
    ).GetSafeNormal();

    TargetLocation = OriginLocation + Direction * MoveDistance;

    StandTrigger->SetBoxExtent(StandTriggerSize);
    ProximityTrigger->SetBoxExtent(ProximityTriggerSize);

    StandTrigger->OnComponentBeginOverlap.AddDynamic(this, &AMovingPlatformActor::OnStandBeginOverlap);
    StandTrigger->OnComponentEndOverlap.AddDynamic(this, &AMovingPlatformActor::OnStandEndOverlap);

    ProximityTrigger->OnComponentBeginOverlap.AddDynamic(this, &AMovingPlatformActor::OnProximityBeginOverlap);
    ProximityTrigger->OnComponentEndOverlap.AddDynamic(this, &AMovingPlatformActor::OnProximityEndOverlap);
}

void AMovingPlatformActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsMoving || bWaiting) return;

    const FVector CurrentLocation = GetActorLocation();
    const FVector Target = bMovingForward ? TargetLocation : OriginLocation;
    const FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, Target, DeltaTime, MoveSpeed);

    SetActorLocation(NewLocation);

    const float DistanceToTarget = FVector::Dist(NewLocation, Target);

    if (DistanceToTarget < 5.f)
    {
        SetActorLocation(Target);

        if (bMovingForward)
        {
            if (bReturnToOrigin)
            {
                bWaiting = true;
                GetWorldTimerManager().SetTimer(
                    WaitTimerHandle,
                    this,
                    &AMovingPlatformActor::StopWaiting,
                    WaitTimeAtDestination,
                    false
                );
            }
            else
            {
                bIsMoving = false;
            }
        }
        else
        {
            // Ha vuelto al origen
            bIsMoving = false;
            bMovingForward = true;

            // Si el jugador sigue cerca, vuelve a moverse
            if (bPlayerNearby)
            {
                StartMoving();
            }
        }
    }
}

void AMovingPlatformActor::StartMoving()
{
    bIsMoving = true;
    bMovingForward = true;
    bWaiting = false;
}

void AMovingPlatformActor::StopWaiting()
{
    bWaiting = false;
    bMovingForward = false;
}

void AMovingPlatformActor::OnStandBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor) return;
    if (!Cast<ACharacter>(OtherActor)) return;

    bPlayerNearby = true;

    if (!bIsMoving)
    {
        StartMoving();
    }
}

void AMovingPlatformActor::OnStandEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    // No paramos aquí — dejamos que termine el ciclo
}

void AMovingPlatformActor::OnProximityBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor) return;
    if (!Cast<ACharacter>(OtherActor)) return;

    bPlayerNearby = true;

    if (!bIsMoving)
    {
        StartMoving();
    }
}

void AMovingPlatformActor::OnProximityEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (!OtherActor) return;
    if (!Cast<ACharacter>(OtherActor)) return;

    bPlayerNearby = false;
}