#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FP_FirstPersonGameMode.generated.h"

UCLASS(minimalapi)
class AFP_FirstPersonGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFP_FirstPersonGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

public:
	void SpawnHost(UWorld* world);
	void MoveToSpawnPoint(class AFP_FirstPersonCharacter* InPlayer);
	void Respawn(class AFP_FirstPersonCharacter* InPlayer);

private:
	TArray<class AFP_FirstPersonCharacter*> RedTeamPlayers;
	TArray<class AFP_FirstPersonCharacter*> BlueTeamPlayers;

	// 스폰포인트에 스폰이 안된 플레이어
	TArray<class AFP_FirstPersonCharacter*> WaitingPlayers;

	TArray<class ACSpawnPoint*> RedTeamSpawnPoints;
	TArray<class ACSpawnPoint*> BlueTeamSpawnPoints;
};



