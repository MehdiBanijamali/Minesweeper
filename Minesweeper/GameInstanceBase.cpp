// Fill out your copyright notice in the Description page of Project Settings.

#include "GameInstanceBase.h"

#include "PlayerPawn.h"
#include "CellsContainer.h"
#include "Lable.h"
#include "GamePlayWidget.h"
#include "Kismet/GameplayStatics.h"

void UGameInstanceBase::Init()
{
	Super::Init();

	if (!FModuleManager::Get().IsModuleLoaded("WebSockets")) 
	{
		FModuleManager::Get().LoadModule("WebSockets");
	}	

	WebSockets = FWebSocketsModule::Get().CreateWebSocket(ServerAddress, ServerProtocol);


	WebSockets->OnConnected().AddUFunction(this, FName("OnConnectToServer"));
	WebSockets->OnConnectionError().AddUFunction(this, FName("OnConnectToServerError"));
	WebSockets->OnConnected().AddUFunction(this, FName("OnConnectToServer"));
	WebSockets->OnConnectionError().AddUFunction(this, FName("OnConnectToServerError"));
	WebSockets->OnClosed().AddUFunction(this, FName("OnDisconnectionServer"));
	WebSockets->OnMessage().AddUFunction(this, FName("OnMessageReceived"));

	WebSockets->Connect();

	// Hold pointer of player class
	APawn* PlayerCharacter = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	PlayerClass = Cast<APlayerPawn>(PlayerCharacter);
	
}

void UGameInstanceBase::OnConnectToServer()
{
	FString NotValid = "Successfuly Connected";
	Notify(NotValid);
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, "Successfuly Connected");
}

void UGameInstanceBase::OnConnectToServerError(const FString& Error)
{
	FString NotValid = Error;
	Notify(NotValid);
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, Error);
}

void UGameInstanceBase::OnDisconnectionServer(int32 StatusCode, const FString& Reason, bool bWasClean)
{
	FString NotValid = Reason;
	Notify(NotValid);
	GEngine->AddOnScreenDebugMessage(-1, 10.f, bWasClean ? FColor::Green : FColor::Red, "Closed" + Reason);

}

void UGameInstanceBase::OnMessageReceived(const FString& Message)
{
	// Slplite received message from server by space 
	TArray<FString> SpliteBySpace;
	Message.ParseIntoArray(SpliteBySpace, TEXT(" "), true);
	// Send message to server
	FString SendMessage = "";

	// Get cammand id from Valid commands map
	int32 CommandId = 0;
	if (ValidCommands.Contains(SpliteBySpace[0]))
	{
		CommandId = ValidCommands[SpliteBySpace[0]];
		switch (CommandId)
		{
			// help
		case 1:
			if (SpliteBySpace.Num() == 1)
			{
				Notify(CommandsDescription);
				break;
			}
			goto default_label;
			// new <l>
		case 2:
			if (SpliteBySpace.Num() == 2)
			{
				int32 LevelNum = FCString::Atoi(*SpliteBySpace[1]);
				NewLevel(LevelNum);
				break;
			}
			goto default_label;
			// map
		case 3:
			if (SpliteBySpace.Num() == 1)
			{
				FString CurrentMap = GetWorld()->GetMapName();

				if (PlayerClass->isCubeMode)
				{
					CurrentMap += " in Cube Mode";
				}
				else
				{
					CurrentMap += " in Flat Mode";
				}
				SendMessage = CurrentMap;
				Notify(SendMessage);
				break;
			}
			goto default_label;
			// open <x> <y>
		case 4:
			if (SpliteBySpace.Num() == 3)
			{
				// Prevent enter alphabetical character
				FString Numbers = SpliteBySpace[1] + SpliteBySpace[2];
				for (char a : Numbers)
				{
					if (int(a) < 48 || int(a) > 57)
					{
						goto default_label;
					}
				}
				if (PlayerClass->CellsConatainer)
				{
					int X = FCString::Atoi(*SpliteBySpace[1]);
					int Y = FCString::Atoi(*SpliteBySpace[2]);
					int32 CellIndex = (Y * PlayerClass->Level) + X;
					OpenCell(CellIndex);
					break;
				}
			}
			goto default_label;
		default:
		default_label:
		{
			FString Wrong = Message;
			Wrong += " is not recognized as an internal or external command.";
			WrongCommand(Wrong);
		}break;
		}
	}

}

// Send Message to server
void UGameInstanceBase::Notify(const FString& Message)
{
	if (WebSockets->IsConnected())
	{
		WebSockets->Send(Message);
	}

}

// Start new level difficulty
void UGameInstanceBase::NewLevel(const int32& NewLevel)
{
	// Valid difficulty is from 1(Easy) to 3(Hard)
	if (NewLevel > 0 && NewLevel < 3)
	{
		if (PlayerClass->CellsConatainer)
		{
			PlayerClass->CellsConatainer->Destroy();
		}
		// Game not start yet
		else
		{
			FString NotValid = "First start the game";
			Notify(NotValid);
			return;
		}
		// Destroy all lable actors
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALable::StaticClass(), FoundActors);
		for (AActor* Lable : FoundActors)
		{
			Lable->Destroy();
		}
		PlayerClass->Level = NewLevel - 1;
		// Stop timer
		PlayerClass->Second = 0;
		PlayerClass->Minutes = 0;
		PlayerClass->GamePlayWidget->RemoveFromParent();
		PlayerClass->StartGame();
	}
	else
	{
		FString NotValid = "Valid difficulty is from 1(Easy) to 3(Hard)";
		Notify(NotValid);
	}
}

// Open cell by index
void UGameInstanceBase::OpenCell(const int& Index)
{
	// Prevent to select a cell towice
	FTransform CellLocatoion;
	PlayerClass->CellsConatainer->InstanceStaticMesh->GetInstanceTransform(Index, CellLocatoion, true);
	if (CellLocatoion.GetScale3D().X == 0)
	{
		FString SelectedBefor = "You already chose this cell";
		Notify(SelectedBefor);
		return;
	}

	// Check current mode
	TArray<FVector2D> AllCheckedIndex;
	if (PlayerClass->isCubeMode)
	{
		int32 SquareOfLevel = PlayerClass->Level * PlayerClass->Level;
		if (Index < 0 || Index >= SquareOfLevel * PlayerClass->Level)
		{
			FString SendMessage = "Not valid x or y";
			Notify(SendMessage);
			return;
		}
		PlayerClass->CountNeighbourMinesCube(Index, AllCheckedIndex);
	}
	else
	{
		int32 SquareOfLevel = PlayerClass->Level * PlayerClass->Level;
		if (Index < 0 || Index >= SquareOfLevel)
		{
			FString SendMessage = "Not valid x or y";
			Notify(SendMessage);
			return;
		}
		PlayerClass->CountNeighbourMinesFlat(Index, AllCheckedIndex);
	}
}

// Received wrong command from server
void UGameInstanceBase::WrongCommand(const FString& Message)
{
	FString WrongCommand = Message;
	WrongCommand += " is not recognized as an internal or external command.";
	Notify(WrongCommand);
}

void UGameInstanceBase::Shutdown()
{
	if (WebSockets->IsConnected())
	{
		WebSockets->Close();
	}
	Super::Shutdown();
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "Connection Closed");

}