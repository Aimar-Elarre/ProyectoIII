#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyThirdPersonCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UUserWidget;
class USoundBase;
struct FInputActionValue;

UCLASS()
class DUNGEONCROWLER_API AMyThirdPersonCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMyThirdPersonCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// Movement / camera
	void Input_Move(const FInputActionValue& Value);
	void Input_Look(const FInputActionValue& Value);

	// Dash / Jump
	void Input_Dash_Started(const FInputActionValue& Value);
	void Input_Jump_Started(const FInputActionValue& Value);
	void Input_Jump_Completed(const FInputActionValue& Value);
	void ResetDash();

	// Drop item
	void Input_Drop(const FInputActionValue& Value);

	// Inventory (TAB)
	void Input_Inventory_Toggle(const FInputActionValue& Value);
	void ShowInventory();
	void HideInventory();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	// Input
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> IMC_Default;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Move;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Look;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_MouseLook;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Jump;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Dash;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Inventory;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Drop;

	// UI
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> InventoryWidgetClass;

	// Dash settings
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dash")
	bool bCanDash = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dash", meta = (ClampMin = "0.0"))
	float DashCooldown = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dash", meta = (ClampMin = "0.0"))
	float DashVelocityMultiplier = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dash")
	TObjectPtr<USoundBase> DashSound = nullptr;

private:
	FTimerHandle DashCooldownHandle;

	UPROPERTY()
	TObjectPtr<UUserWidget> InventoryWidgetInstance = nullptr;

	UPROPERTY()
	bool bInventoryOpen = false;
};