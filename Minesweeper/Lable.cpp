// Fill out your copyright notice in the Description page of Project Settings.

#include "Lable.h"
#include "Components/PointLightComponent.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>

// Sets default values
ALable::ALable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	// Emmite light around number
	PointLightComp = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLightComp"));
	PointLightComp->SetupAttachment(RootComponent);
	PointLightComp->LightColor = FColor(75, 249, 255, 1);
	PointLightComp->CastShadows = false;
	PointLightComp->Intensity = 1000;
	PointLightComp->SetRelativeLocation(FVector(0, 0, -20));

	// Show number of mines
	NumberTextRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("NumberTextRender"));
	NumberTextRender->WorldSize = 70.f;
	NumberTextRender->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	NumberTextRender->SetupAttachment(PointLightComp);
	NumberTextRender->SetRelativeLocation(FVector(0, 0, -10));
	
}

// Called when the game starts or when spawned
void ALable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Always face to camera
	FRotator Rotate = (UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetActorForwardVector()*-1).Rotation();
	SetActorRotation(Rotate);

}

