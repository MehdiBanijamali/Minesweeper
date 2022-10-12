// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPawn.h"
#include "Components/SceneComponent.h"
#include "Uobject/ConstructorHelpers.h"
#include <Camera/CameraComponent.h>
#include "GameFramework/SpringArmComponent.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "MinesweeperGameModeBase.h"
#include "GamePlayWidget.h"
#include "CellsContainer.h"
#include "Lable.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = true;

	// Setup Scene Component
	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	RootComponent = SceneComp;

	// To move camera around this static mesh and spawn cells container in this location
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));

	// Setup Camera
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(SceneComp);
	SpringArmComp->TargetArmLength = 3000.f;
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetRelativeLocation(FVector(0.f, 30.f, 200.f));
	CameraComponent->bUsePawnControlRotation = true;
	CameraComponent->SetupAttachment(SpringArmComp);

	// Menu widget
	const ConstructorHelpers::FClassFinder<UUserWidget> MenuWidgetPath(TEXT("WidgetBlueprint'/Game/Widget/MenuWidget.MenuWidget_C'"));
	MenuWidget = CreateWidget<UUserWidget>(GetWorld(), MenuWidgetPath.Class);
	// Game play widget
	const ConstructorHelpers::FClassFinder<UUserWidget> GameWidgetPath(TEXT("WidgetBlueprint'/Game/Widget/GamePlayWidgetBP.GamePlayWidgetBP_C'"));
	GamePlayWidget = CreateWidget<UUserWidget>(GetWorld(), GameWidgetPath.Class);
	// To change values of widget like timer
	GamePlayWidgetInstance = Cast<UGamePlayWidget>(GamePlayWidget);

}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	MenuWidget->AddToViewport();

	// To use player controller every where need
	PlController = UGameplayStatics::GetPlayerController(this, 0);
	
	// Show mouse cursor during play
	PlController->bShowMouseCursor = true;

}

// Setup Scene
void APlayerPawn::StartGame()
{
	// Get mines count and cells count from Game mode
	AMinesweeperGameModeBase* GM = Cast<AMinesweeperGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	Mines = GM->Grid[Level][1];
	Level = GM->Grid[Level][0];

	if (!isCubeMode)
	{
		Mines /= GM->FlatMode;
	}

	// Setup Game play Widget
	GamePlayWidget->AddToViewport();
	GamePlayWidgetInstance->MinesCount->SetText(FText::FromString(FString::FromInt(Mines)));
	// Start timer
	GetWorldTimerManager().SetTimer(SetTimeWidgetHandle, this, &APlayerPawn::SetTimerWidget, 1, true, 1);

	// To Put mines on Grid
	PlaceMines();

	// Spawn Cells container class
	FActorSpawnParameters SpawnParameters;
	FTransform SpawnTransform = StaticMeshComp->GetRelativeTransform();
	CellsConatainer = GetWorld()->SpawnActor<ACellsContainer>(ACellsContainer::StaticClass(), SpawnTransform, SpawnParameters);

}

// Put Mines on Gride
void APlayerPawn::PlaceMines()
{
	// To ensure no mine left from previous game
	MinesContainer.Empty();
	int RandomBomb;
	for (int i = 0; i < Mines;)
	{
		// In Cube mode placed mines in all layers
		if (isCubeMode)
		{
			RandomBomb = FMath::RandRange(0, (Level * Level * Level));
		}
		else
		{
			RandomBomb = FMath::RandRange(0, (Level * Level));
		}

		if (!MinesContainer.Contains(RandomBomb))
		{
			MinesContainer.Add(RandomBomb);
			++i;

		}
	}

}

// Set timer widget
void APlayerPawn::SetTimerWidget()
{
	Second += 1;
	if (Second > 59)
	{
		Second = 0;
		Minutes += 1;
	}
	FNumberFormattingOptions NumFormat;
	NumFormat.MinimumIntegralDigits = 2;
	GamePlayWidgetInstance->TimerTxt->SetText(FText::FromString(FString((FText::AsNumber(Minutes, &NumFormat)).ToString() + TEXT(":") + FString((FText::AsNumber(Second, &NumFormat)).ToString()))));
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Select cell by left click
	PlayerInputComponent->BindAction("CellSelect", IE_Pressed, this, &APlayerPawn::OnClick);
	PlayerInputComponent->BindAction("CellSelect", IE_Released, this, &APlayerPawn::OnRelease);
	// Select Mine by right click
	PlayerInputComponent->BindAction("Mines", IE_Pressed, this, &APlayerPawn::FoundMines);
	// Turn by mouse axis while press left clisk
	PlayerInputComponent->BindAxis("Turn", this, &APlayerPawn::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerPawn::LookUp);
	PlayerInputComponent->BindAxis("CameraZoom", this, &APlayerPawn::CameraZoom);

}

// On left click press
void APlayerPawn::OnClick()
{
	isTouch = PlController->GetMousePosition(BeginTouchVector.X, BeginTouchVector.Y);

}

// On left click release
void APlayerPawn::OnRelease()
{
	isTouch = PlController->GetMousePosition(EndTouchVector.X, EndTouchVector.Y);

	// To ensure is it drag or click
	FVector2D DeltaTouchLocation = (EndTouchVector - BeginTouchVector);
	if (DeltaTouchLocation.Size() < 1)
	{
		// Get mouse position on screen to world
		FVector mouseLocation, mouseDirection;
		PlController->DeprojectMousePositionToWorld(mouseLocation, mouseDirection);

		// Draw line trace to hit cells
		FVector StartLine = mouseLocation;
		FVector EndLine = (mouseDirection * 10000) + (mouseLocation);
		FHitResult Hit;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		bool isHit;
		isHit = GetWorld()->LineTraceSingleByChannel(Hit, StartLine, EndLine, ECollisionChannel::ECC_GameTraceChannel3, QueryParams);
		// Is hit to cell container
		if (isHit)
		{
			if (Hit.GetComponent()->GetName() == "InstanceStaticMesh")
			{
				// Count click on cells
				++CurrentMove;
				GamePlayWidgetInstance->MoveCount->SetText(FText::FromString(FString::FromInt(CurrentMove)));
				// If click on mines, you lose
				int Index = Hit.Item;
				if (MinesContainer.Contains(Index))
				{
					//Game over
					EndGame(false);
				}
				// Count number of mines around selected cell
				else
				{	
					// To ensure cells not check towice
					TArray<FVector2D> AllCheckedIndex;
					if (isCubeMode)
					{
						CountNeighbourMinesCube(Index, AllCheckedIndex);
					}
					else
					{
						CountNeighbourMinesFlat(Index, AllCheckedIndex);
					}
				}

			}
		}

	}
}

// Count all mines around the Index in Cube mode
void APlayerPawn::CountNeighbourMinesCube(const int& Index, TArray<FVector2D>& AllCheckedIndex)
{
	// Prevent double check
	for (FVector2D all : AllCheckedIndex)
	{
		if (all[0] == Index)
		{
			return;
		}
	}

	// Calculte min and max Y in grid of each layer in Cube mode
	int minY = (Index / (Level * Level)) * Level;
	int maxY = minY + Level;

	// Convert Index to X, Y
	int X = Index % Level;
	int Y = Index / Level;
	// Check X, Y for out of Grid range
	if (X < 0 || X >= Level || Y < minY || Y >= maxY)
	{
		return;
	}
	// Check index for out of Cells range
	int SquareOfLevel = Level * Level;
	if (Index < 0 || Index >= SquareOfLevel * Level)
	{
		return;
	}

	// Count Number of Mines
	int CountMines = 0;
	for (FVector2D vec : Map) {
		int nx = X + vec[0];
		int ny = Y + vec[1];
		if (nx < 0 || nx >= Level || ny < minY || ny >= maxY) continue;
		int CurrenLayer = (ny * Level) + nx;
		int NextLayer = SquareOfLevel + CurrenLayer;
		int PreLayer = CurrenLayer - SquareOfLevel;
		for (int Bomb : MinesContainer)
		{
			// Is any mines on top or button layer of cube
			if (Bomb == CurrenLayer || Bomb == NextLayer || Bomb == PreLayer)
			{
				++CountMines;
			}
		}
	}
	// is any mines on exact top or buttom of selected Index 
	if (MinesContainer.Contains(Index - SquareOfLevel) || MinesContainer.Contains(Index + SquareOfLevel))
	{
		++CountMines;
	}

	// Create a Vector to show all mines around the cells
	AllCheckedIndex.Add(FVector2D(Index, CountMines));

	// Found mine around selected cell
	if (CountMines > 0)
	{
		SpawnLable(Index, CountMines);

	}
	// No mine exist around selected cell
	else 
	{
		// If not mines found remove the Cell
		FTransform Transform;
		FVector CellsPosition(0, 0, 0);
		Transform.SetScale3D(CellsPosition);
		CellsConatainer->InstanceStaticMesh->UpdateInstanceTransform(Index, Transform, false, true);
		// Search untile found a cell that has a mine in neighbour
		for (FVector2D vec : Map) 
		{
			int nx = X + vec[0];
			int ny = Y + vec[1];
			if (nx < 0 || nx >= Level || ny < minY || ny >= maxY)
			{
				continue;
			}
			int CurrenLayer = (ny * Level) + nx;
			int NextLayer = SquareOfLevel + CurrenLayer;
			int PreLayer = CurrenLayer - SquareOfLevel;

			CountNeighbourMinesCube(CurrenLayer, AllCheckedIndex);
			CountNeighbourMinesCube(NextLayer, AllCheckedIndex);
			CountNeighbourMinesCube(PreLayer, AllCheckedIndex);
		}

		CountNeighbourMinesCube(Index - SquareOfLevel, AllCheckedIndex);
		CountNeighbourMinesCube(Index + SquareOfLevel, AllCheckedIndex);
	}

}

// Count all mines around the Index in Flat mode
void APlayerPawn::CountNeighbourMinesFlat(const int& Index, TArray<FVector2D>& AllCheckedIndex)
{
	// Prevent double check
	for (FVector2D all : AllCheckedIndex)
	{
		if (all[0] == Index)
		{
			return;
		}
	}
	// Convert Index to X, Y
	int X = Index % Level;
	int Y = Index / Level;
	
	// Check X, Y for out of Grid range
	if (X < 0 || X >= Level || Y < 0 || Y >= Level)
	{
		return;
	}

	// Count Number of Mines
	int CountMines = 0;
	for (FVector2D vec : Map) {
		int nx = X + vec[0];
		int ny = Y + vec[1];
		if (nx < 0 || nx >= Level || ny < 0 || ny >= Level)
		{
			continue;
		}
		int CurrenLayer = (ny * Level) + nx;
		for (int Bomb : MinesContainer)
		{
			// Is any mines on top or button layer of cube
			if (Bomb == CurrenLayer)
			{
				++CountMines;
			}
		}
	}
	
	// Create a Vector to show all mines around the cells
	AllCheckedIndex.Add(FVector2D(Index, CountMines));

	// Found mine around selected cell
	if (CountMines > 0)
	{
		SpawnLable(Index, CountMines);

	}
	// No mine exist around selected cell
	else
	{
		// If not mines found remove the Cell
		FTransform Transform;
		FVector CellsPosition(0, 0, 0);
		Transform.SetScale3D(CellsPosition);
		CellsConatainer->InstanceStaticMesh->UpdateInstanceTransform(Index, Transform, false, true);
		// Search untile found a cell that has a mine in neighbour
		for (FVector2D vec : Map)
		{
			int nx = X + vec[0];
			int ny = Y + vec[1];
			if (nx < 0 || nx >= Level || ny < 0 || ny >= Level)
			{
				continue;
			}
			int NewIndex = (ny * Level) + nx;
			CountNeighbourMinesFlat(NewIndex, AllCheckedIndex);
		}
	}

}

// Right click on Grid to determine cell is a mine container
void APlayerPawn::FoundMines()
{
	FVector mouseLocation, mouseDirection;
	PlController->DeprojectMousePositionToWorld(mouseLocation, mouseDirection);

	// Draw line trace by right click
	FVector StartLine = mouseLocation;
	FVector EndLine = (mouseDirection * 10000) + (mouseLocation);
	FHitResult Hit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	bool isHit;
	isHit = GetWorld()->LineTraceSingleByChannel(Hit, StartLine, EndLine, ECollisionChannel::ECC_GameTraceChannel3, QueryParams);
	// If hit Mines container
	if (isHit)
	{
		if (Hit.GetComponent()->GetName() == "InstanceStaticMesh")
		{
			// Unselect mine selected
			if (FoundedMines.Contains(Hit.Item))
			{
				TArray<float> CustomData;
				CustomData.Init(0.f, 3);
				CustomData[0] = 0.f;
				CustomData[1] = 0.f;
				CustomData[2] = 0.f;
				CellsConatainer->InstanceStaticMesh->SetCustomData(Hit.Item, CustomData, true);
				FoundedMines.Remove(Hit.Item);
				++Mines;
				GamePlayWidgetInstance->MinesCount->SetText(FText::FromString(FString::FromInt(Mines)));
			}
			// Select mine cell
			else
			{
				TArray<float> CustomData;
				CustomData.Init(0.f, 3);
				CustomData[0] = 20.f;
				CustomData[1] = 0.f;
				CustomData[2] = 0.f;
				CellsConatainer->InstanceStaticMesh->SetCustomData(Hit.Item, CustomData, true);
				FoundedMines.Add(Hit.Item);
				--Mines;
				GamePlayWidgetInstance->MinesCount->SetText(FText::FromString(FString::FromInt(Mines)));
			}
			// Player select wrong mines cell
			bool isAllMinesFound = true;
			if (Mines == 0)
			{
				for (int Mine : FoundedMines)
				{
					if (!MinesContainer.Contains(Mine))
					{
						isAllMinesFound = false;
						break;
					}
				}
				// All mine founded and win
				if (isAllMinesFound)
				{
					EndGame(true);
				}
			}
		}
	}

}

// Show number of mines around the cell
void APlayerPawn::SpawnLable(const int& CellIndex, const int& BombCount)
{
	// Get transform of selected mine
	FTransform CellLocatoion;
	CellsConatainer->InstanceStaticMesh->GetInstanceTransform(CellIndex, CellLocatoion, true);
	// Spawn Lable actor
	FActorSpawnParameters GhostSpawn;
	AActor* lable = GetWorld()->SpawnActor<AActor>(ALable::StaticClass(), CellLocatoion, GhostSpawn);
	lable->AttachToComponent(CellsConatainer->InstanceStaticMesh, FAttachmentTransformRules::KeepWorldTransform);
	// Set number of mines around the cell
	ALable* l = Cast<ALable>(lable);
	l->NumberTextRender->SetText(FText::FromString(FString::FromInt(BombCount)));
	
	// Remove selected cell
	FTransform Transform;
	FVector CellsPosition(0, 0, 0);
	Transform.SetScale3D(CellsPosition);
	CellsConatainer->InstanceStaticMesh->UpdateInstanceTransform(CellIndex, Transform, false, true);

}
// Win or Game over
void APlayerPawn::EndGame(bool Result)
{
	// Stop timer
	GetWorld()->GetTimerManager().ClearTimer(SetTimeWidgetHandle);
	
	DisableInput(PlController);

	// Show result
	GamePlayWidgetInstance->EndGameCanvas->SetVisibility(ESlateVisibility::Visible);
	if (Result)
	{
		GamePlayWidgetInstance->ResultTxt->SetText(FText::FromString("Congratulations, You Won!"));
	}
	else
	{
		GamePlayWidgetInstance->ResultTxt->SetText(FText::FromString("You Lose"));
	}

}

// Automated solving algorithm
void APlayerPawn::ShowAllMines()
{
	for (int MineIndex : MinesContainer) {

		TArray<float> CustomData;
		CustomData.Init(0.f, 3);
		CustomData[0] = 0.f;
		CustomData[1] = 20.f;
		CustomData[2] = 0.f;
		CellsConatainer->InstanceStaticMesh->SetCustomData(MineIndex, CustomData, true);
	}
}

void APlayerPawn::Turn(float Axis)
{
	if (isTouch) 
	{
		AddControllerYawInput(Axis);
	}

}

void APlayerPawn::LookUp(float Axis)
{
	if (isTouch) 
	{
		AddControllerPitchInput(Axis);
	}

}

void APlayerPawn::CameraZoom(const float Value)
{
	if (Value == 0.f && !Controller)
	{
		return;
	}
	const float NewTargetArmLength = SpringArmComp->TargetArmLength + Value * ZoomStep;
	SpringArmComp->TargetArmLength = FMath::Clamp(NewTargetArmLength, MinZoomLength, MaxZoomLength);

}


