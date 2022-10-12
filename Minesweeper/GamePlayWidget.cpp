// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePlayWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "PlayerPawn.h"

void UGamePlayWidget::NativeConstruct()
{
	EndGameCanvas->SetVisibility(ESlateVisibility::Hidden);
	BackToMenuBtn->OnClicked.AddDynamic(this, &UGamePlayWidget::Back);
	ShowAllMines->OnClicked.AddDynamic(this, &UGamePlayWidget::ShowMines);
}

// Back to Main menu
void UGamePlayWidget::Back()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(*GetWorld()->GetName()), false);

}

// Automated solving algorithm
void UGamePlayWidget::ShowMines()
{
	APawn* PlayerCharacter = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	APlayerPawn* PlayerClass = Cast<APlayerPawn>(PlayerCharacter);
	PlayerClass->ShowAllMines();

}



