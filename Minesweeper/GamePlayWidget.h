// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "GamePlayWidget.generated.h"

/** Show timer, mines and move counter */
UCLASS()
class MINESWEEPER_API UGamePlayWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void NativeConstruct() override;

	// Timer , Move count , Mines count
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TimerTxt;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MoveCount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MinesCount;

	// Wint or Lose
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* EndGameCanvas;

	UPROPERTY(meta = (BindWidget))
	class UButton* BackToMenuBtn;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ResultTxt;
	UFUNCTION()
	void Back();

	// Menu Widget BluePrint refrence
	UPROPERTY(BlueprintReadWrite)
	UClass* MenuWidgetClass;

	// To Show Menu widget
	UPROPERTY(BlueprintReadWrite)
	UUserWidget* MenuWidget;

	UPROPERTY(meta = (BindWidget))
	class UButton* ShowAllMines;

	UFUNCTION()
	void ShowMines();
};
