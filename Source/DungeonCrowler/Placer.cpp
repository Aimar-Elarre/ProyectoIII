#include "Placer.h"
#include "Components/SphereComponent.h"

APlacer::APlacer()
    : SpawnCount(1)
{
    // No queremos replicar este actor en red
    bReplicates = false;
    
    // El GameState manejará el lifecycle
    RootComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootSphere"));
    
    VisualizationSphere = RootComponent;
    VisualizationSphere->SetSphereRadius(50.0f);
    VisualizationSphere->SetCollisionEnabled(ECC_WorldStatic);
    
    // Visualización solo en el editor
    #if WITH_EDITOR
    VisualizationSphere->ShapeColor = FColor::Green;
    #endif
}

void APlacer::BeginPlay()
{
    Super::BeginPlay();
    
    // Las acciones de spawn las maneja ACrawGS en su BeginPlay
    // Este Placer será destruido por el GameState tras ser procesado
}
