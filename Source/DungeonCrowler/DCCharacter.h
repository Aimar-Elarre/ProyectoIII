#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DCCharacter.generated.h"

UCLASS()
class DUNGEONCROWLER_API ADCCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ADCCharacter();

protected:
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    void Dash();

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    class UInputAction* DashAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    class UInputMappingContext* MappingContext;

private:
    bool bCanDash = true;
};
