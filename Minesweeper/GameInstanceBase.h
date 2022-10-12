// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "WebSocketsModule.h"
#include "IWebSocket.h"
#include "GameInstanceBase.generated.h"

/** WebSocket */
UCLASS()
class MINESWEEPER_API UGameInstanceBase : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

public:

	TSharedPtr<IWebSocket> WebSockets;

	// I didn't know how to connect a secure websocket and didn't have internet access to find out.
	const FString ServerAddress = "ws://hometask.eg1236.com/game1/";
	const FString ServerProtocol = TEXT("ws");
	//FString ServerAddress = "ws://localhost:8080";

	UFUNCTION()
	void OnConnectToServer();

	UFUNCTION()
	void OnConnectToServerError(const FString& Error);

	UFUNCTION()
	void OnDisconnectionServer(int32 StatusCode, const FString& Reason, bool bWasClean);

	UFUNCTION()
	void OnMessageReceived(const FString& Message);

	UFUNCTION()
	virtual void Shutdown() override;

	// Hold valid command from server
	UPROPERTY()
	TMap<FString, int32> ValidCommands{ {"help",1},{"new",2},{"map",3},{"open",4} };

	// String show all valid command with help keyword
	UPROPERTY()
	FString CommandsDescription = "help - lists other commands\n"
		"new <l> - chooses the game level\n"
		"map - returns the current map\n"
		"open <x> <y> - opens the cell at coordinates (<x>, <y>)";

	// Send Message to server
	UFUNCTION()
	void Notify(const FString& Message);

	// Change level difficulty
	UFUNCTION()
	void NewLevel(const int32& NewLevel);

	// Choose one cell by x, y 
	UFUNCTION()
	void OpenCell(const int& Index);
	UFUNCTION()
	// Wrong command from server
	void WrongCommand(const FString& Message);

	// Pointer to player class
	UPROPERTY()
	class APlayerPawn* PlayerClass;
};
