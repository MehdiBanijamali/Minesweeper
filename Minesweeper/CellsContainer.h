// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CellsContainer.generated.h"

/** Grid Container */
UCLASS()
class MINESWEEPER_API ACellsContainer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACellsContainer();

	// Set Scene 
	UPROPERTY(VisibleAnywhere, Category = "Base")
	USceneComponent* SceneComp;

	// To create Grid
	UPROPERTY(VisibleAnywhere, Category = "Base")
	UInstancedStaticMeshComponent* InstanceStaticMesh;

	// Space between cells
	const int Space = 120;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
