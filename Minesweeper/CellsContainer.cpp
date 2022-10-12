// Fill out your copyright notice in the Description page of Project Settings.

#include "CellsContainer.h"
#include "PlayerPawn.h"
#include "Components/SceneComponent.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>

// Sets default values
ACellsContainer::ACellsContainer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	// Scene component
	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	RootComponent = SceneComp;

	InstanceStaticMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstanceStaticMesh"));
	InstanceStaticMesh->SetupAttachment(SceneComp);
	InstanceStaticMesh->SetMobility(EComponentMobility::Movable);
	InstanceStaticMesh->SetCollisionProfileName("OverlapAll");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubePath(TEXT("StaticMesh'/Game/Asset/CubeMesh.CubeMesh'"));
	InstanceStaticMesh->SetStaticMesh(CubePath.Object);
	InstanceStaticMesh->NumCustomDataFloats = 3;
	InstanceStaticMesh->MarkRenderStateDirty();
	
}

// Called when the game starts or when spawned
void ACellsContainer::BeginPlay()
{
	Super::BeginPlay();

	APawn* PlayerCharacter = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	APlayerPawn* PlayerClass = Cast<APlayerPawn>(PlayerCharacter);

	int NumberOfGrid = PlayerClass->Level;
	// Flat Mode
	int Layer = 1;
	if (PlayerClass->isCubeMode)
	{
		// Cube Mode
		Layer = NumberOfGrid;
	}
	
	// Set Grid
	FTransform Transform;
	// Make Cube
	for(int i=0; i < Layer; ++i)
	{
		// Make Layer
		for (int j = 0; j < (NumberOfGrid * NumberOfGrid); ++j) {
			FVector CellsPosition((400 -(j / NumberOfGrid) * Space), 400 -((j % NumberOfGrid) * Space), i * Space);
			Transform.SetLocation(CellsPosition);
			InstanceStaticMesh->AddInstance(Transform);
		}
	}	

}

// Called every frame
void ACellsContainer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

