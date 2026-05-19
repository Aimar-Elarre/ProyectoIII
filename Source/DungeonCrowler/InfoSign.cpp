#include "InfoSign.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/BoxComponent.h"
#include "MyPlayerCharacter.h"

AInfoSign::AInfoSign()
{
    PrimaryActorTick.bCanEverTick = false;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    SignMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SignMesh"));
    SignMesh->SetupAttachment(Root);
    SignMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    SignMesh->SetCollisionResponseToAllChannels(ECR_Block);

    SignTextRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("SignTextRender"));
    SignTextRender->SetupAttachment(Root);
    SignTextRender->SetRelativeLocation(FVector(5.0f, 0.0f, 80.0f));
    SignTextRender->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
    SignTextRender->SetHorizontalAlignment(EHTA_Center);
    SignTextRender->SetVerticalAlignment(EVRTA_TextCenter);
    SignTextRender->SetWorldSize(TextWorldSize);
    SignTextRender->SetTextRenderColor(TextColor);
    SignTextRender->SetText(SignText);

    InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
    InteractionBox->SetupAttachment(Root);
    InteractionBox->SetBoxExtent(FVector(120.0f, 120.0f, 120.0f));
    InteractionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    InteractionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    InteractionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AInfoSign::BeginPlay()
{
    Super::BeginPlay();

    UpdateSignText();

    if (InteractionBox)
    {
        InteractionBox->OnComponentBeginOverlap.AddDynamic(this, &AInfoSign::OnInteractionBoxBeginOverlap);
        InteractionBox->OnComponentEndOverlap.AddDynamic(this, &AInfoSign::OnInteractionBoxEndOverlap);
    }
}

void AInfoSign::UpdateSignText()
{
    if (!SignTextRender)
    {
        return;
    }

    SignTextRender->SetText(SignText);
    SignTextRender->SetWorldSize(TextWorldSize);
    SignTextRender->SetTextRenderColor(TextColor);
}

void AInfoSign::OnInteractionBoxBeginOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
    AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(OtherActor);

    if (Player)
    {
        Player->ShowHintMessage(TEXT("Lee el cartel"));
    }
}

void AInfoSign::OnInteractionBoxEndOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex
)
{
    AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(OtherActor);

    if (Player)
    {
        Player->HideHintMessage();
    }
}