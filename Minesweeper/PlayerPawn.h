// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"

/** Camera class to rotate and select Cells */
UCLASS()
class MINESWEEPER_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerPawn();

	// Set Scene, Camera 
	UPROPERTY(VisibleAnywhere, Category = "Base")
	class USceneComponent* SceneComp;

	UPROPERTY(VisibleAnywhere, Category = "Base")
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, Category = "Base")
	class UCameraComponent* CameraComponent;
	// Location to spawn Cells container
	UPROPERTY(VisibleAnywhere, Category = "Base")
	class UStaticMeshComponent* StaticMeshComp;

	// To use player controller every where need
	UPROPERTY()
	APlayerController* PlController;

	// Setup Camera Movement
	void Turn(float Axis);
	void LookUp(float Axis);
	void CameraZoom(const float Value);
	const float ZoomStep = 20.f;
	const float MinZoomLength = 1000.f;
	const float MaxZoomLength = 4000.f;

	// Handl click input
	void OnClick();
	void OnRelease();
	// To ensure player click on cells not drag
	FVector2D BeginTouchVector;
	FVector2D EndTouchVector;
	// Mouse drag or not
	bool isTouch = false;

	// Select founded mine by right click
	void FoundMines();

	// Setup Scene
	void StartGame();
	UPROPERTY()
	class ACellsContainer* CellsConatainer;

	// Put Mines on Gride
	void PlaceMines();
	UPROPERTY()
	TArray<int> MinesContainer;
	UPROPERTY()
	TArray<int> FoundedMines;
	// Difficulty
	int32 Level = 0;
	// Number of Mines
	int Mines = 0;
	// Cube mode or Flat mode
	bool isCubeMode = true;
	
	// Widget Handle
	UPROPERTY()
	class UUserWidget* MenuWidget;
	UPROPERTY()
	class UUserWidget* GamePlayWidget;
	UPROPERTY()
	class UGamePlayWidget* GamePlayWidgetInstance;

	// Setup timer
	FTimerHandle SetTimeWidgetHandle;
	void SetTimerWidget();
	int32 Second = 0;
	int32 Minutes = 0;
	// Count player's select cells
	int CurrentMove = 0;

	// Game play
	// Count placed mines around selected cell in Cube mode
	void CountNeighbourMinesCube(const int &Index, TArray<FVector2D>& AllCheckedIndex);
	// Count placed mines around selected cell in Flat mode
	void CountNeighbourMinesFlat(const int &Index, TArray<FVector2D>& AllCheckedIndex);
	// Add x,y to Map to get all neighbours cells
	TArray<FVector2D>Map{ {1,0},{0,1},{-1,0},{0,-1},{1,-1},{-1,1},{1,1},{-1,-1} };
	// Show number of mines around cell
	void SpawnLable(const int &CellIndex, const int &BombCount);
	// Win or Lose
	void EndGame(bool Result);

	// Automated solving algorithm
	UFUNCTION()
	void ShowAllMines();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
