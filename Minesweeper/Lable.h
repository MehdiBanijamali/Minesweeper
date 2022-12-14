// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TextRenderComponent.h"
#include "Lable.generated.h"

/** Show number of mines around cells */
UCLASS()
class MINESWEEPER_API ALable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALable();

	// Emmite light around number
	UPROPERTY(VisibleAnywhere, Category = "Base")
	class UPointLightComponent* PointLightComp;
	// Show number of mines
	UPROPERTY(VisibleAnywhere, Category = "Base")
	UTextRenderComponent* NumberTextRender;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
