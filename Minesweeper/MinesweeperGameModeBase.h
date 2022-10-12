// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MinesweeperGameModeBase.generated.h"

/** Difine Grid size and number of mines in each level */
#define Easy 6
#define Medium 8
#define Hard 10

#define EasyMines 30
#define MediumMines 60
#define HardMines 120


UCLASS()
class MINESWEEPER_API AMinesweeperGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:

	const TArray<FVector2D> Grid{ {Easy,EasyMines},{Medium,MediumMines},{Hard,HardMines} };

	// In Flat Mode, Mines divided to this
	const int32 FlatMode= 4;
};
