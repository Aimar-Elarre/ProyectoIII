#include "MyThirdPersonCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

AMyThirdPersonCharacter::AMyThirdPersonCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
}

void AMyThirdPersonCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Mapping Context (Enhanced Input)
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LP = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (IMC_Default)
				{
					Subsystem->AddMappingContext(IMC_Default, 0);
				}
			}
		}
	}
}

void AMyThirdPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EIC) return;

	if (IA_Move)
	{
		EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AMyThirdPersonCharacter::Input_Move);
	}

	if (IA_Look)
	{
		EIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AMyThirdPersonCharacter::Input_Look);
	}
	if (IA_MouseLook)
	{
		EIC->BindAction(IA_MouseLook, ETriggerEvent::Triggered, this, &AMyThirdPersonCharacter::Input_Look);
	}

	if (IA_Jump)
	{
		EIC->BindAction(IA_Jump, ETriggerEvent::Started, this, &AMyThirdPersonCharacter::Input_Jump_Started);
		EIC->BindAction(IA_Jump, ETriggerEvent::Completed, this, &AMyThirdPersonCharacter::Input_Jump_Completed);
	}

	if (IA_Dash)
	{
		EIC->BindAction(IA_Dash, ETriggerEvent::Started, this, &AMyThirdPersonCharacter::Input_Dash_Started);
	}

	// ✅ INVENTARIO (TAB)
	if (IA_Inventory)
	{
		EIC->BindAction(IA_Inventory, ETriggerEvent::Started, this, &AMyThirdPersonCharacter::Input_Inventory_Toggle);
	}
}

void AMyThirdPersonCharacter::Input_Move(const FInputActionValue& Value)
{
	const FVector2D MoveAxis = Value.Get<FVector2D>();
	if (!Controller) return;

	const FRotator ControlRot = Controller->GetControlRotation();
	const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);

	const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

	AddMovementInput(Forward, MoveAxis.Y);
	AddMovementInput(Right, MoveAxis.X);
}

void AMyThirdPersonCharacter::Input_Look(const FInputActionValue& Value)
{
	const FVector2D LookAxis = Value.Get<FVector2D>();
	if (!Controller) return;

	AddControllerYawInput(LookAxis.X);
	AddControllerPitchInput(LookAxis.Y);
}

void AMyThirdPersonCharacter::Input_Jump_Started(const FInputActionValue& /*Value*/)
{
	Jump();
}

void AMyThirdPersonCharacter::Input_Jump_Completed(const FInputActionValue& /*Value*/)
{
	StopJumping();
}

void AMyThirdPersonCharacter::Input_Dash_Started(const FInputActionValue& /*Value*/)
{
	if (!bCanDash) return;

	bCanDash = false;

	const FVector LaunchVel = GetVelocity() * DashVelocityMultiplier;
	LaunchCharacter(LaunchVel, true, true);

	if (DashSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DashSound, GetActorLocation());
	}

	GetWorldTimerManager().ClearTimer(DashCooldownHandle);
	GetWorldTimerManager().SetTimer(DashCooldownHandle, this, &AMyThirdPersonCharacter::ResetDash, DashCooldown, false);
}

void AMyThirdPersonCharacter::ResetDash()
{
	bCanDash = true;
}

// =======================
// ✅ INVENTARIO (TAB)
// =======================

void AMyThirdPersonCharacter::Input_Inventory_Toggle(const FInputActionValue& /*Value*/)
{
	UE_LOG(LogTemp, Warning, TEXT("TAB FUNCIONA"));
	if (bInventoryOpen) HideInventory();
	else ShowInventory();
	
}

void AMyThirdPersonCharacter::ShowInventory()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	// Crear widget si no existe
	if (!InventoryWidgetInstance && InventoryWidgetClass)
	{
		InventoryWidgetInstance = CreateWidget<UUserWidget>(PC, InventoryWidgetClass);
	}

	if (InventoryWidgetInstance && !InventoryWidgetInstance->IsInViewport())
	{
		InventoryWidgetInstance->AddToViewport(10);
	}

	bInventoryOpen = true;

	// Opcional: pausar el juego
	// UGameplayStatics::SetGamePaused(this, true);

	// Cambiar a modo UI
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

void AMyThirdPersonCharacter::HideInventory()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	if (InventoryWidgetInstance && InventoryWidgetInstance->IsInViewport())
	{
		InventoryWidgetInstance->RemoveFromParent();
	}

	bInventoryOpen = false;

	// Opcional: reanudar
	// UGameplayStatics::SetGamePaused(this, false);

	// Volver a juego
	FInputModeGameOnly Mode;
	PC->SetInputMode(Mode);
	PC->bShowMouseCursor = false;
}