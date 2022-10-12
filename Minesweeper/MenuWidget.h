// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuWidget.generated.h"

/** Widget to set Map and Difficulty */
UCLASS()
class MINESWEEPER_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// Default Constructor
	void NativeConstruct() override;

	UPROPERTY()
	class APlayerPawn* PlayerClass;
	
	// Set Map(Cube mode or Flat mode)
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* MenuCanvas;
	UPROPERTY(meta = (BindWidget))
	class UButton* CubeModeBtn;
	UPROPERTY(meta = (BindWidget))
	class UButton* FlatModeBtn;
	UPROPERTY(meta = (BindWidget))
	class UButton* BackBtn;

	UFUNCTION()
	void CubeMode();
	UFUNCTION()
	void FlatMode();
	UFUNCTION()
	void Back();

	// Set difficulty
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* DifficultyCanvas;
	UPROPERTY(meta = (BindWidget))
	class UButton* EasyBtn;
	UPROPERTY(meta = (BindWidget))
	class UButton* MediumBtn;
	UPROPERTY(meta = (BindWidget))
	class UButton* HardBtn;
	
	UFUNCTION()
	void EasyLevel();
	UFUNCTION()
	void MediumLevel();
	UFUNCTION()
	void HardLevel();

};
