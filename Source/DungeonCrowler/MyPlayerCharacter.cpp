#include "MyPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PickupItemActor.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"

AMyPlayerCharacter::AMyPlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);

    GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

void AMyPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth;
}

void AMyPlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AMyPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &AMyPlayerCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AMyPlayerCharacter::MoveRight);

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMyPlayerCharacter::StartJump);
    PlayerInputComponent->BindAction("Drop", IE_Pressed, this, &AMyPlayerCharacter::DropItem);
}

void AMyPlayerCharacter::MoveForward(float Value)
{
    AddMovementInput(GetActorForwardVector(), Value);
}

void AMyPlayerCharacter::MoveRight(float Value)
{
    AddMovementInput(GetActorRightVector(), Value);
}

void AMyPlayerCharacter::StartJump()
{
    Jump();
}

void AMyPlayerCharacter::StopJump()
{
    StopJumping();
}

void AMyPlayerCharacter::TakeDamageCustom(float DamageAmount)
{
    CurrentHealth -= DamageAmount;

    if (CurrentHealth <= 0)
    {
        Die();
    }
}

void AMyPlayerCharacter::Die()
{
    UE_LOG(LogTemp, Warning, TEXT("Jugador muerto"));
}

void AMyPlayerCharacter::DropItem()
{
    if (ItemsCarried <= 0 || !PickupItemClass) return;

    FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 120.f;
    GetWorld()->SpawnActor<APickupItemActor>(PickupItemClass, SpawnLocation, FRotator::ZeroRotator);

    ItemsCarried--;
    UpdateMovementSpeed();
}

void AMyPlayerCharacter::AddCarriedItem(int32 Amount)
{
    ItemsCarried += Amount;
    if (ItemsCarried < 0)
    {
        ItemsCarried = 0;
    }

    UpdateMovementSpeed();
}

int32 AMyPlayerCharacter::GetItemsCarried() const
{
    return ItemsCarried;
}

float AMyPlayerCharacter::GetCurrentHealthValue() const
{
    return CurrentHealth;
}

bool AMyPlayerCharacter::IsDashUnlocked() const
{
    return bDashUnlocked;
}

void AMyPlayerCharacter::UnlockDash()
{
    bDashUnlocked = true;
    UE_LOG(LogTemp, Warning, TEXT("Dash desbloqueado"));
}

void AMyPlayerCharacter::ShowHintMessage(const FString& Message)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, Message);
    }
}

void AMyPlayerCharacter::HideHintMessage()
{
    if (GEngine)
    {
        GEngine->ClearOnScreenDebugMessages();
    }
}

void AMyPlayerCharacter::SetLastCheckpoint(FVector NewLocation)
{
    LastCheckpointLocation = NewLocation;
    bHasCheckpoint = true;
    UE_LOG(LogTemp, Warning, TEXT("Checkpoint guardado"));
}

void AMyPlayerCharacter::RespawnAtCheckpoint()
{
}

void AMyPlayerCharacter::StartRun()
{
}

void AMyPlayerCharacter::StopRun()
{
}

void AMyPlayerCharacter::Turn(float Value)
{
    AddControllerYawInput(Value);
}

void AMyPlayerCharacter::LookUp(float Value)
{
    AddControllerPitchInput(Value);
}

void AMyPlayerCharacter::StartCrouch()
{
}

void AMyPlayerCharacter::StopCrouch()
{
}

void AMyPlayerCharacter::StartSlide()
{
}

void AMyPlayerCharacter::StopSlide()
{
}

void AMyPlayerCharacter::UpdateMovementSpeed()
{
    float Multiplier = 1.0f - (ItemsCarried * SpeedPenaltyPerItem);
    Multiplier = FMath::Clamp(Multiplier, MinSpeedMultiplier, 1.0f);

    GetCharacterMovement()->MaxWalkSpeed = 600.f * Multiplier;
}

float AMyPlayerCharacter::GetStaminaPercent() const
{
    return 1.0f;
}