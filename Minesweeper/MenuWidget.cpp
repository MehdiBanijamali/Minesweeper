// Fill out your copyright notice in the Description page of Project Settings.

#include "MenuWidget.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "Components/CanvasPanel.h"
#include "Components/Button.h"
#include "PlayerPawn.h"

void UMenuWidget::NativeConstruct()
{
	APawn* PlayerCharacter = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	PlayerClass = Cast<APlayerPawn>(PlayerCharacter);

	CubeModeBtn->OnClicked.AddDynamic(this, &UMenuWidget::CubeMode);
	FlatModeBtn->OnClicked.AddDynamic(this, &UMenuWidget::FlatMode);
	BackBtn->OnClicked.AddDynamic(this, &UMenuWidget::Back);

	DifficultyCanvas->SetVisibility(ESlateVisibility::Hidden);
	EasyBtn->OnClicked.AddDynamic(this, &UMenuWidget::EasyLevel);
	MediumBtn->OnClicked.AddDynamic(this, &UMenuWidget::MediumLevel);
	HardBtn->OnClicked.AddDynamic(this, &UMenuWidget::HardLevel);

}

// Cube mode select
void UMenuWidget::CubeMode()
{
	PlayerClass->isCubeMode = true;
	MenuCanvas->SetVisibility(ESlateVisibility::Hidden);
	DifficultyCanvas->SetVisibility(ESlateVisibility::Visible);

}

// Flat mode select
void UMenuWidget::FlatMode()
{
	PlayerClass->isCubeMode = false;
	MenuCanvas->SetVisibility(ESlateVisibility::Hidden);
	DifficultyCanvas->SetVisibility(ESlateVisibility::Visible);

}

// Back to change Map
void UMenuWidget::Back()
{
	DifficultyCanvas->SetVisibility(ESlateVisibility::Hidden);
	MenuCanvas->SetVisibility(ESlateVisibility::Visible);

}

void UMenuWidget::EasyLevel()
{
	PlayerClass->Level = 0;
	PlayerClass->StartGame();
	RemoveFromParent();

}

void UMenuWidget::MediumLevel()
{
	PlayerClass->Level = 1;
	PlayerClass->StartGame();
	RemoveFromParent();

}

void UMenuWidget::HardLevel()
{
	PlayerClass->Level = 2;
	PlayerClass->StartGame();
	RemoveFromParent();

}
