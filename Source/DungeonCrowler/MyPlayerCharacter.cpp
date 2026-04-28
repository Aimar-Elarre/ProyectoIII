#include "MyPlayerCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PickupItemActor.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "InventoryWidget.h"
#include "ItemData.h"
#include "Perception/AISense_Hearing.h"

AMyPlayerCharacter::AMyPlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
    GetCapsuleComponent()->SetGenerateOverlapEvents(true);

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 0.f;
    SpringArm->bUsePawnControlRotation = true;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);
    Camera->bUsePawnControlRotation = false;
    Camera->SetRelativeLocation(FVector(0.f, 0.f, 64.f));

    bUseControllerRotationYaw = true;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
    GetCharacterMovement()->JumpZVelocity = 420.f;
    GetCharacterMovement()->GravityScale = 1.6f;
    GetCharacterMovement()->AirControl = 0.5f;
    GetCharacterMovement()->FallingLateralFriction = 1.0f;
    GetCharacterMovement()->BrakingDecelerationFalling = 1500.f;

    JumpMaxHoldTime = 0.0f;

    CurrentCapsuleHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

    FootstepAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FootstepAudioComponent"));
    FootstepAudioComponent->SetupAttachment(RootComponent);
    FootstepAudioComponent->bAutoActivate = false;
    FootstepAudioComponent->bIsUISound = false;

    InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
}

void AMyPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;
    CurrentStamina = MaxStamina;

    GetCharacterMovement()->JumpZVelocity = JumpStrength;
    UpdateMovementSpeed();

    if (GetMesh())
    {
        CurrentMeshZ = GetMesh()->GetRelativeLocation().Z;
        MeshStandingZ = CurrentMeshZ;

        CurrentMeshScale = GetMesh()->GetRelativeScale3D();
        MeshStandingScale = CurrentMeshScale;
    }

    if (Camera)
    {
        Camera->SetFieldOfView(NormalFOV);
    }

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (PC->PlayerCameraManager)
        {
            PC->PlayerCameraManager->ViewPitchMin = -40.f;
            PC->PlayerCameraManager->ViewPitchMax = 70.f;
        }
    }

    if (IsLocallyControlled() && PlayerHUDClass)
    {
        PlayerHUD = CreateWidget<UMyPlayerHUD>(GetWorld(), PlayerHUDClass);
        if (PlayerHUD)
        {
            PlayerHUD->AddToViewport();
            PlayerHUD->SetStaminaVisibility(bSprintUnlocked);
        }
    }

    if (FootstepAudioComponent && FootstepSound)
    {
        FootstepAudioComponent->SetSound(FootstepSound);
    }

    RefreshLegacyCarryFromInventory();
    UpdateMovementSpeed();
}

void AMyPlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    const bool bOnGround = GetCharacterMovement()->IsMovingOnGround();

    if (bOnGround)
    {
        bHasJumped = false;
    }

    FVector HorizontalVelocity = GetVelocity();
    HorizontalVelocity.Z = 0.f;

    if (bIsRunning && (!bOnGround || HorizontalVelocity.SizeSquared() <= 1.f))
    {
        bIsRunning = false;
        UpdateMovementSpeed();
    }

    if (bIsRunning)
    {
        CurrentStamina -= StaminaDrainRate * DeltaTime;

        if (CurrentStamina <= 0.f)
        {
            CurrentStamina = 0.f;
            bIsRunning = false;
            UpdateMovementSpeed();
        }
    }
    else
    {
        if (CurrentStamina < MaxStamina)
        {
            CurrentStamina += StaminaRegenRate * DeltaTime;
            CurrentStamina = FMath::Clamp(CurrentStamina, 0.f, MaxStamina);
        }
    }

    if (!bIsRunning && bRunKeyHeld)
    {
        const bool bCanTryRun =
            bSprintUnlocked &&
            !bIsSliding &&
            !bIsCrouching &&
            bOnGround &&
            CurrentStamina > 0.f;

        if (bCanTryRun)
        {
            FVector RunVelocity = GetVelocity();
            RunVelocity.Z = 0.f;

            if (RunVelocity.SizeSquared() > 1.f)
            {
                bIsRunning = true;
                HideHintMessage();
                UpdateMovementSpeed();
            }
        }
    }

    RefreshLegacyCarryFromInventory();

    if (ItemsCarried != LastItemsCarriedForMovement)
    {
        LastItemsCarriedForMovement = ItemsCarried;
        UpdateMovementSpeed();
    }

    if (PlayerHUD)
    {
        PlayerHUD->UpdateStamina(GetStaminaPercent());
        PlayerHUD->UpdateCarry(ItemsCarried, FMath::Max(ItemsCarried, 1));
    }

    const float TargetHeight = bIsCrouching ? CrouchHeight : StandingHeight;

    CurrentCapsuleHeight = FMath::FInterpTo(
        CurrentCapsuleHeight,
        TargetHeight,
        DeltaTime,
        CrouchSpeed
    );

    if (GetMesh())
    {
        const float TargetMeshZ = bIsCrouching ? MeshCrouchingZ : MeshStandingZ;

        CurrentMeshZ = FMath::FInterpTo(
            CurrentMeshZ,
            TargetMeshZ,
            DeltaTime,
            CrouchSpeed
        );

        FVector MeshLocation = GetMesh()->GetRelativeLocation();
        MeshLocation.Z = CurrentMeshZ;
        GetMesh()->SetRelativeLocation(MeshLocation);

        const FVector TargetScale = bIsCrouching ? MeshCrouchingScale : MeshStandingScale;

        CurrentMeshScale = FMath::VInterpTo(
            CurrentMeshScale,
            TargetScale,
            DeltaTime,
            CrouchSpeed
        );

        GetMesh()->SetRelativeScale3D(CurrentMeshScale);
    }

    GetCapsuleComponent()->SetCapsuleHalfHeight(CurrentCapsuleHeight, true);

    if (Camera)
    {
        const float TargetFOV = bIsRunning ? RunFOV : NormalFOV;

        CurrentDashFOVOffset = FMath::FInterpTo(
            CurrentDashFOVOffset,
            0.f,
            DeltaTime,
            DashFOVRecoverSpeed
        );

        const float FinalTargetFOV = TargetFOV + CurrentDashFOVOffset;
        const float NewFOV = FMath::FInterpTo(
            Camera->FieldOfView,
            FinalTargetFOV,
            DeltaTime,
            FOVInterpSpeed
        );

        Camera->SetFieldOfView(NewFOV);
    }

    if (!bOnGround || bIsDead || bIsSliding)
    {
        StopFootstepAudio();
    }
}

void AMyPlayerCharacter::DropSpecificItem(const UItemData* ItemData)
{
    if (!InventoryComponent || !ItemData || !ItemData->PickupActorClass)
    {
        return;
    }

    const FVector SpawnLocation = GetActorLocation() + GetActorUpVector() * 50.f + GetActorForwardVector() * 60.f;
    const FRotator SpawnRotation = FRotator::ZeroRotator;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    APickupItemActor* SpawnedPickup = GetWorld()->SpawnActor<APickupItemActor>(
        ItemData->PickupActorClass,
        SpawnLocation,
        SpawnRotation,
        SpawnParams
    );

    if (!SpawnedPickup)
    {
        UE_LOG(LogTemp, Warning, TEXT("No se pudo spawnear el pickup"));
        return;
    }

    SpawnedPickup->SetItemData(ItemData);

    const FVector DropImpulse = GetActorForwardVector() * 350.f + FVector(0.f, 0.f, 300.f);
    SpawnedPickup->SpawnAsDropped(DropImpulse);

    InventoryComponent->RemoveItem(ItemData, 1);
    RefreshLegacyCarryFromInventory();
    UpdateMovementSpeed();

    UE_LOG(LogTemp, Warning, TEXT("Objeto soltado desde widget: %s"), *ItemData->DisplayName.ToString());
}

void AMyPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &AMyPlayerCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AMyPlayerCharacter::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &AMyPlayerCharacter::Turn);
    PlayerInputComponent->BindAxis("LookUp", this, &AMyPlayerCharacter::LookUp);

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMyPlayerCharacter::StartJump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMyPlayerCharacter::StopJump);

    PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AMyPlayerCharacter::StartRun);
    PlayerInputComponent->BindAction("Run", IE_Released, this, &AMyPlayerCharacter::StopRun);

    PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMyPlayerCharacter::StartCrouch);
    PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AMyPlayerCharacter::StopCrouch);

    PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &AMyPlayerCharacter::Dash);
    PlayerInputComponent->BindAction("Kill", IE_Pressed, this, &AMyPlayerCharacter::KillPlayer);
    PlayerInputComponent->BindAction("Drop", IE_Pressed, this, &AMyPlayerCharacter::DropItem);
    PlayerInputComponent->BindKey(EKeys::V, IE_Pressed, this, &AMyPlayerCharacter::TryInteractPickup);

    PlayerInputComponent->BindKey(EKeys::F1, IE_Pressed, this, &AMyPlayerCharacter::Debug_UnlockSprint);
    PlayerInputComponent->BindKey(EKeys::F6, IE_Pressed, this, &AMyPlayerCharacter::Debug_UnlockDash);
    PlayerInputComponent->BindKey(EKeys::F7, IE_Pressed, this, &AMyPlayerCharacter::Debug_FillStamina);

    FInputActionBinding& InventoryBinding =
        PlayerInputComponent->BindAction("Inventory", IE_Pressed, this, &AMyPlayerCharacter::Input_Inventory_Toggle);

    InventoryBinding.bExecuteWhenPaused = true;
}

void AMyPlayerCharacter::MoveForward(float Value)
{
    UpdateFootstepAudio(Value);

    if (Controller && Value != 0.0f)
    {
        const FRotator ControlRot = Controller->GetControlRotation();
        const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);

        const FVector ForwardDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
        AddMovementInput(ForwardDir, Value);

        const float Loudness = bIsRunning ? 0.8f : 0.4f;
        MakeMovementNoise(Loudness);
    }
}

void AMyPlayerCharacter::MoveRight(float Value)
{
    if (Controller && Value != 0.0f)
    {
        const FRotator ControlRot = Controller->GetControlRotation();
        const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);

        const FVector RightDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
        AddMovementInput(RightDir, Value);

        const float Loudness = bIsRunning ? 0.8f : 0.4f;
        MakeMovementNoise(Loudness);

        if (GetCharacterMovement()->IsMovingOnGround() && !bIsSliding && !bIsDead)
        {
            UpdateFootstepAudio(1.0f);
        }
    }
}

void AMyPlayerCharacter::TryInteractPickup()
{
    if (NearbyPickup)
    {
        NearbyPickup->TryPickup(this);
    }
}

void AMyPlayerCharacter::SetNearbyPickup(APickupItemActor* NewPickup)
{
    NearbyPickup = NewPickup;

    if (NearbyPickup)
    {
        ShowHintMessage(TEXT("Pulsa V para recoger"));
    }
}

void AMyPlayerCharacter::ClearNearbyPickup(APickupItemActor* PickupToClear)
{
    if (NearbyPickup == PickupToClear)
    {
        NearbyPickup = nullptr;
        HideHintMessage();
    }
}

void AMyPlayerCharacter::StartJump()
{
    if (!GetCharacterMovement()->IsMovingOnGround())
    {
        return;
    }

    Jump();
    bHasJumped = true;

    MakeMovementNoise(1.0f);

    const float CurrentTime = GetWorld()->GetTimeSeconds();
    FootstepBlockedUntil = CurrentTime + FootstepBlockAfterJump;
    LastFootstepTime = CurrentTime;

    if (JumpSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, JumpSound, GetActorLocation());
    }
}

void AMyPlayerCharacter::StopJump()
{
    StopJumping();
}

void AMyPlayerCharacter::StartRun()
{
    bRunKeyHeld = true;

    if (!bSprintUnlocked) return;
    if (bIsSliding) return;
    if (bIsCrouching) return;
    if (CurrentStamina <= 0.f) return;
    if (!GetCharacterMovement()->IsMovingOnGround()) return;

    FVector HorizontalVelocity = GetVelocity();
    HorizontalVelocity.Z = 0.f;

    if (HorizontalVelocity.SizeSquared() <= 1.f) return;

    HideHintMessage();

    bIsRunning = true;
    UpdateMovementSpeed();
}

void AMyPlayerCharacter::StopRun()
{
    bRunKeyHeld = false;
    bIsRunning = false;
    UpdateMovementSpeed();
}

void AMyPlayerCharacter::StartCrouch()
{
    FVector HorizontalVelocity = GetVelocity();
    HorizontalVelocity.Z = 0.f;

    if (HorizontalVelocity.Size() > MinSlideSpeed && GetCharacterMovement()->IsMovingOnGround())
    {
        StartSlide();
        return;
    }

    bIsCrouching = true;
    bIsRunning = false;
    UpdateMovementSpeed();
}

void AMyPlayerCharacter::StopCrouch()
{
    if (bIsSliding)
    {
        return;
    }

    bIsCrouching = false;
    UpdateMovementSpeed();
}

void AMyPlayerCharacter::StartSlide()
{
    if (bIsSliding) return;

    bIsSliding = true;
    bIsCrouching = true;
    bIsRunning = false;

    OriginalGroundFriction = GetCharacterMovement()->GroundFriction;
    GetCharacterMovement()->GroundFriction = SlideFriction;

    FVector SlideDirection = GetVelocity();
    SlideDirection.Z = 0.f;
    SlideDirection = SlideDirection.GetSafeNormal();

    LaunchCharacter(SlideDirection * SlideImpulse, true, false);

    GetWorldTimerManager().SetTimer(
        SlideTimerHandle,
        this,
        &AMyPlayerCharacter::StopSlide,
        SlideDuration,
        false
    );
}

void AMyPlayerCharacter::StopSlide()
{
    bIsSliding = false;
    bIsCrouching = true;

    GetCharacterMovement()->GroundFriction = OriginalGroundFriction;
    UpdateMovementSpeed();
}

void AMyPlayerCharacter::Dash()
{
    if (!bDashUnlocked) return;
    if (!bCanDash) return;
    if (!bHasJumped) return;
    if (GetCharacterMovement()->IsMovingOnGround()) return;

    bCanDash = false;

    const FRotator ControlRot = Controller ? Controller->GetControlRotation() : GetActorRotation();
    FVector DashDir = FRotationMatrix(ControlRot).GetUnitAxis(EAxis::X);
    DashDir.Z = 0.f;
    DashDir.Normalize();

    LaunchCharacter(DashDir * DashStrength, true, false);

    CurrentDashFOVOffset = DashFOVBoost;

    if (DashSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, DashSound, GetActorLocation());
    }

    MakeMovementNoise(1.2f);

    GetWorldTimerManager().SetTimer(
        DashCooldownHandle,
        this,
        &AMyPlayerCharacter::ResetDash,
        DashCooldown,
        false
    );
}

void AMyPlayerCharacter::ResetDash()
{
    if (bDashUnlocked)
    {
        bCanDash = true;
    }
}

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
    RefreshLegacyCarryFromInventory();

    float Multiplier = 1.0f - (ItemsCarried * SpeedPenaltyPerItem);
    Multiplier = FMath::Clamp(Multiplier, MinSpeedMultiplier, 1.0f);

    float BaseSpeed = bIsRunning ? RunSpeed : WalkSpeed;

    if (bIsCrouching)
    {
        BaseSpeed *= 0.55f;
    }

    const float FinalSpeed = BaseSpeed * Multiplier;
    GetCharacterMovement()->MaxWalkSpeed = FinalSpeed;

    const float FinalJump = JumpStrength * Multiplier;
    GetCharacterMovement()->JumpZVelocity = FinalJump;

    UE_LOG(LogTemp, Warning, TEXT("Items: %d | Speed: %f | Jump: %f"),
        ItemsCarried, FinalSpeed, FinalJump);

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            12345,
            1.5f,
            FColor::Yellow,
            FString::Printf(
                TEXT("Items=%d | Speed=%.1f | Jump=%.1f"),
                ItemsCarried,
                FinalSpeed,
                FinalJump
            )
        );
    }
}

void AMyPlayerCharacter::UpdateFootstepAudio(float ForwardValue)
{
    if (!FootstepAudioComponent || !FootstepSound) return;

    const bool bMovingInput = FMath::Abs(ForwardValue) > 0.01f;
    const bool bCanPlay =
        bMovingInput &&
        GetCharacterMovement()->IsMovingOnGround() &&
        !bIsSliding &&
        !bIsDead;

    if (bCanPlay)
    {
        if (!FootstepAudioComponent->IsPlaying())
        {
            FootstepAudioComponent->Play();
        }
    }
    else
    {
        if (FootstepAudioComponent->IsPlaying())
        {
            FootstepAudioComponent->Stop();
        }
    }
}

void AMyPlayerCharacter::StopFootstepAudio()
{
    if (FootstepAudioComponent && FootstepAudioComponent->IsPlaying())
    {
        FootstepAudioComponent->Stop();
    }
}

void AMyPlayerCharacter::MakeMovementNoise(float Loudness)
{
    UAISense_Hearing::ReportNoiseEvent(
        GetWorld(),
        GetActorLocation(),
        Loudness,
        this
    );
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

    StopFootstepAudio();
    GetCharacterMovement()->DisableMovement();

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        DisableInput(PC);
    }

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            2.f,
            FColor::Red,
            TEXT("ESTAS MUERTO")
        );
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
            GEngine->AddOnScreenDebugMessage(
                -1,
                3.f,
                FColor::Red,
                TEXT("NO HAY CHECKPOINT GUARDADO")
            );
        }
        return;
    }

    SetActorLocation(LastCheckpointLocation);

    CurrentHealth = MaxHealth;
    CurrentStamina = MaxStamina;
    bIsDead = false;
    bIsRunning = false;
    bIsSliding = false;
    bIsCrouching = false;

    GetCharacterMovement()->SetMovementMode(MOVE_Walking);
    GetCharacterMovement()->GroundFriction = OriginalGroundFriction;
    UpdateMovementSpeed();

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        EnableInput(PC);
    }
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
    if (!Entry.ItemData || !Entry.ItemData->PickupActorClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("ItemData o PickupActorClass inválido"));
        return;
    }

    const FVector SpawnLocation = GetActorLocation() + GetActorUpVector() * 50.f + GetActorForwardVector() * 60.f;
    const FRotator SpawnRotation = FRotator::ZeroRotator;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    APickupItemActor* SpawnedPickup = GetWorld()->SpawnActor<APickupItemActor>(
        Entry.ItemData->PickupActorClass,
        SpawnLocation,
        SpawnRotation,
        SpawnParams
    );

    if (!SpawnedPickup)
    {
        UE_LOG(LogTemp, Warning, TEXT("No se pudo spawnear el pickup"));
        return;
    }

    SpawnedPickup->SetItemData(Entry.ItemData);

    const FVector DropImpulse = GetActorForwardVector() * 350.f + FVector(0.f, 0.f, 300.f);
    SpawnedPickup->SpawnAsDropped(DropImpulse);

    InventoryComponent->RemoveItem(Entry.ItemData, 1);
    RefreshLegacyCarryFromInventory();
    UpdateMovementSpeed();

    UE_LOG(LogTemp, Warning, TEXT("Objeto soltado: %s"), *Entry.ItemData->DisplayName.ToString());
}

void AMyPlayerCharacter::AddCarriedItem(int32 Amount)
{
    ItemsCarried += Amount;
    if (ItemsCarried < 0)
    {
        ItemsCarried = 0;
    }

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
    if (PlayerHUD)
    {
        PlayerHUD->ShowHint(Message);
    }

    GetWorldTimerManager().ClearTimer(HintTimerHandle);
    GetWorldTimerManager().SetTimer(
        HintTimerHandle,
        this,
        &AMyPlayerCharacter::HideHintMessage,
        5.0f,
        false
    );
}

void AMyPlayerCharacter::ShowInventorySecondHint()
{
    ShowHintMessage(TEXT("Cada objeto pesa y tu mochila tiene un límite.\nSuelta lo que no necesites: cuanto más cargues, peor te moverás."));
}

void AMyPlayerCharacter::HideHintMessage()
{
    if (PlayerHUD)
    {
        PlayerHUD->HideHint();
    }
}

float AMyPlayerCharacter::GetStaminaPercent() const
{
    if (MaxStamina <= 0.f)
    {
        return 0.f;
    }

    return CurrentStamina / MaxStamina;
}

// ============================
// DINERO / MONEY
// ============================

void AMyPlayerCharacter::AddMoney(float Amount)
{
    if (Amount <= 0.f) return;

    CurrentMoney += Amount;
    UE_LOG(LogTemp, Warning, TEXT("Dinero añadido: +%.2f | Total: %.2f"), Amount, CurrentMoney);
}

float AMyPlayerCharacter::GetCurrentMoney() const
{
    return CurrentMoney;
}

void AMyPlayerCharacter::Input_Inventory_Toggle()
{
    if (!bInventoryUnlocked)
    {
        return;
    }

    if (bInventoryOpen)
    {
        HideInventory();
    }
    else
    {
        ShowInventory();
    }
}

bool AMyPlayerCharacter::IsInventoryUnlocked() const
{
    return bInventoryUnlocked;
}

void AMyPlayerCharacter::UnlockInventory()
{
    bInventoryUnlocked = true;

    ShowHintMessage(TEXT("Pulsa Tab para abrir el inventario y recoger objetos del suelo"));

    GetWorldTimerManager().ClearTimer(InventoryTutorialSecondHintHandle);
    GetWorldTimerManager().SetTimer(
        InventoryTutorialSecondHintHandle,
        this,
        &AMyPlayerCharacter::ShowInventorySecondHint,
        3.5f,
        false
    );
}

void AMyPlayerCharacter::Debug_UnlockSprint()
{
    UnlockSprint();
}

void AMyPlayerCharacter::Debug_UnlockDash()
{
    UnlockDash();
}

void AMyPlayerCharacter::Debug_FillStamina()
{
    CurrentStamina = MaxStamina;

    if (PlayerHUD)
    {
        PlayerHUD->UpdateStamina(GetStaminaPercent());
    }

    UE_LOG(LogTemp, Warning, TEXT("DEBUG: Stamina rellenada"));
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

    UGameplayStatics::SetGamePaused(GetWorld(), true);

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

    UGameplayStatics::SetGamePaused(GetWorld(), false);

    FInputModeGameOnly Mode;
    PC->SetInputMode(Mode);
    PC->bShowMouseCursor = false;
}

bool AMyPlayerCharacter::IsSprintUnlocked() const
{
    return bSprintUnlocked;
}

void AMyPlayerCharacter::UnlockSprint()
{
    bSprintUnlocked = true;

    if (PlayerHUD)
    {
        PlayerHUD->SetStaminaVisibility(true);
    }

    ShowHintMessage(TEXT("Usa Shift para poder correr"));
}

void AMyPlayerCharacter::RefreshLegacyCarryFromInventory()
{
    if (!InventoryComponent)
    {
        return;
    }

    const TArray<FInventoryEntry> Items = InventoryComponent->GetItemsAsArray();

    int32 TotalCount = 0;
    for (const FInventoryEntry& Entry : Items)
    {
        TotalCount += Entry.Quantity;
    }

    ItemsCarried = TotalCount;
}
