#include "MyPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PickupItemActor.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"
#include "InventoryWidget.h"
#include "ItemData.h"

AMyPlayerCharacter::AMyPlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);

    GetCharacterMovement()->MaxWalkSpeed = 600.f;

    // Inventario nuevo
    InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
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
    PlayerInputComponent->BindAction("Inventory", IE_Pressed, this, &AMyPlayerCharacter::Input_Inventory_Toggle);
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
    if (bIsDead) return;

    CurrentHealth -= DamageAmount;
    CurrentHealth = FMath::Clamp(CurrentHealth, 0.f, MaxHealth);

    if (CurrentHealth <= 0.f)
    {
        Die();
    }
}

void AMyPlayerCharacter::Die()
{
    if (bIsDead) return;

    bIsDead = true;

    UE_LOG(LogTemp, Warning, TEXT("Personaje muerto"));

    GetCharacterMovement()->DisableMovement();

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        DisableInput(PC);
    }

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("ESTAS MUERTO"));
    }

    GetWorldTimerManager().SetTimer(
        RespawnTimerHandle,
        this,
        &AMyPlayerCharacter::RespawnAtCheckpoint,
        2.0f,
        false
    );
}

void AMyPlayerCharacter::KillPlayer()
{
    if (bIsDead) return;

    CurrentHealth = 0.f;
    Die();
}

void AMyPlayerCharacter::DropItem()
{
    if (!InventoryComponent) return;

    TArray<FInventoryEntry> Items = InventoryComponent->GetItemsAsArray();
    if (Items.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Inventario vacío"));
        return;
    }

    const FInventoryEntry& Entry = Items[0];
    if (!Entry.ItemData || !Entry.ItemData->PickupActorClass) return;

    FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 120.f + FVector(0.f, 0.f, 40.f);
    FRotator SpawnRotation = FRotator::ZeroRotator;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    GetWorld()->SpawnActor<APickupItemActor>(
        Entry.ItemData->PickupActorClass,
        SpawnLocation,
        SpawnRotation,
        SpawnParams
    );

    // Ahora sí elimina del inventario
    InventoryComponent->RemoveItem(Entry.ItemData, 1);

    UE_LOG(LogTemp, Warning, TEXT("Objeto soltado: %s"), *Entry.ItemData->DisplayName.ToString());
}


void AMyPlayerCharacter::AddCarriedItem(int32 Amount)
{
    ItemsCarried += Amount;
    if (ItemsCarried < 0) ItemsCarried = 0;
    UpdateMovementSpeed();
    UE_LOG(LogTemp, Warning, TEXT("Items ahora: %d"), ItemsCarried);
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
    bCanDash = true;
    ShowHintMessage(TEXT("Dash desbloqueado. Pulsa Q en el aire"));
}

void AMyPlayerCharacter::ShowHintMessage(const FString& Message)
{
    if (PlayerHUD) PlayerHUD->ShowHint(Message);
}

void AMyPlayerCharacter::HideHintMessage()
{
    if (PlayerHUD) PlayerHUD->HideHint();
}

void AMyPlayerCharacter::SetLastCheckpoint(FVector NewLocation)
{
    LastCheckpointLocation = NewLocation;
    bHasCheckpoint = true;
    UE_LOG(LogTemp, Warning, TEXT("CHECKPOINT GUARDADO EN: %s"), *LastCheckpointLocation.ToString());
}

void AMyPlayerCharacter::RespawnAtCheckpoint()
{
    UE_LOG(LogTemp, Warning, TEXT("RESPAWN CHECKPOINT FLAG: %d"), bHasCheckpoint);

    if (!bHasCheckpoint)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("NO HAY CHECKPOINT GUARDADO"));
        }
        return;
    }

    SetActorLocation(LastCheckpointLocation);
    CurrentHealth = MaxHealth;
    bIsDead = false;

    GetCharacterMovement()->SetMovementMode(MOVE_Walking);
    UpdateMovementSpeed();

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        EnableInput(PC);
    }
}

void AMyPlayerCharacter::StartRun() {}
void AMyPlayerCharacter::StopRun() {}
void AMyPlayerCharacter::StartCrouch() {}
void AMyPlayerCharacter::StopCrouch() {}
void AMyPlayerCharacter::StartSlide() {}
void AMyPlayerCharacter::StopSlide() {}
void AMyPlayerCharacter::Dash() {}

void AMyPlayerCharacter::Turn(float Value)
{
    AddControllerYawInput(Value * MouseSensitivity);
}

void AMyPlayerCharacter::LookUp(float Value)
{
    AddControllerPitchInput(Value * MouseSensitivity);
}

void AMyPlayerCharacter::UpdateMovementSpeed()
{
    float Multiplier = 1.0f - (ItemsCarried * SpeedPenaltyPerItem);
    Multiplier = FMath::Clamp(Multiplier, MinSpeedMultiplier, 1.0f);
    GetCharacterMovement()->MaxWalkSpeed = 600.f * Multiplier;
}

float AMyPlayerCharacter::GetStaminaPercent() const
{
    if (MaxStamina <= 0.f) return 0.f;
    return CurrentStamina / MaxStamina;
}

// =======================
// INVENTARIO (TAB)
// =======================

void AMyPlayerCharacter::Input_Inventory_Toggle()
{
    if (bInventoryOpen) HideInventory();
    else ShowInventory();
}

void AMyPlayerCharacter::ShowInventory()
{
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC) return;

    if (!InventoryWidgetInstance && InventoryWidgetClass)
    {
        InventoryWidgetInstance = CreateWidget<UUserWidget>(PC, InventoryWidgetClass);

        if (UInventoryWidget* InvWidget = Cast<UInventoryWidget>(InventoryWidgetInstance))
        {
            InvWidget->InitInventory(InventoryComponent);
        }
    }

    if (InventoryWidgetInstance && !InventoryWidgetInstance->IsInViewport())
    {
        InventoryWidgetInstance->AddToViewport(10);
    }

    bInventoryOpen = true;

    FInputModeGameAndUI Mode;
    Mode.SetHideCursorDuringCapture(false);
    Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

    if (InventoryWidgetInstance)
    {
        Mode.SetWidgetToFocus(InventoryWidgetInstance->TakeWidget());
    }

    PC->SetInputMode(Mode);
    PC->bShowMouseCursor = true;
}

void AMyPlayerCharacter::HideInventory()
{
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC) return;

    if (InventoryWidgetInstance && InventoryWidgetInstance->IsInViewport())
    {
        InventoryWidgetInstance->RemoveFromParent();
    }

    bInventoryOpen = false;

    FInputModeGameOnly Mode;
    PC->SetInputMode(Mode);
    PC->bShowMouseCursor = false;
}
