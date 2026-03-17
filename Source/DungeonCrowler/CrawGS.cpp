#include "CrawGS.h"
#include "Placer.h"
#include "Kismet/GameplayStatics.h"

ACrawGS::ACrawGS()
{
    // No necesitamos replicación para la lógica de spawning
    bReplicates = false;
}

void ACrawGS::BeginPlay()
{
    Super::BeginPlay();

    // Procesamos todos los Placeres del mapa
    ProcessAllPlacers();
}

void ACrawGS::ProcessAllPlacers()
{
    // Validar que haya actor classes configuradas
    if (ActorPoolClasses.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("ACrawGS: ActorPoolClasses está vacío. No se spawnearán actores."));
        return;
    }

    // Buscar todos los APlacer en el mundo
    FoundPlacers.Empty();
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlacer::StaticClass(), 
                                          reinterpret_cast<TArray<AActor*>&>(FoundPlacers));

    if (FoundPlacers.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("ACrawGS: No se encontraron Placeres en el mapa."));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("ACrawGS: Encontrados %d Placeres. Iniciando spawning..."), FoundPlacers.Num());

    // Procesar cada Placer
    for (APlacer* Placer : FoundPlacers)
    {
        if (IsValid(Placer))
        {
            ProcessPlacer(Placer);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("ACrawGS: Spawning completado."));
}

TSubclassOf<AActor> ACrawGS::GetRandomActorClass() const
{
    if (ActorPoolClasses.Num() == 0)
    {
        return nullptr;
    }

    // Seleccionar aleatoriamente un índice
    int32 RandomIndex = FMath::RandRange(0, ActorPoolClasses.Num() - 1);
    return ActorPoolClasses[RandomIndex];
}

void ACrawGS::ProcessPlacer(APlacer* Placer)
{
    if (!IsValid(Placer))
    {
        return;
    }

    // Obtener la clase a spawnear
    TSubclassOf<AActor> ActorClassToSpawn = GetRandomActorClass();
    if (!ActorClassToSpawn)
    {
        UE_LOG(LogTemp, Error, TEXT("ACrawGS: No se pudo seleccionar una clase válida para spawnear."));
        Placer->Destroy();
        return;
    }

    // Spawnear el actor en la posición del Placer
    FVector SpawnLocation = Placer->GetActorLocation();
    FRotator SpawnRotation = Placer->GetActorRotation();

    AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
        ActorClassToSpawn,
        SpawnLocation,
        SpawnRotation
    );

    if (SpawnedActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("ACrawGS: Actor spawneado exitosamente en %s"), *SpawnLocation.ToString());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ACrawGS: Error al spawnear actor de clase %s"), *ActorClassToSpawn->GetName());
    }

    // Destruir el Placer tras spawnear
    Placer->Destroy();
}
