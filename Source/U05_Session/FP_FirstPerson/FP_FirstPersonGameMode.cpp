#include "FP_FirstPersonGameMode.h"
#include "FP_FirstPersonHUD.h"
#include "FP_FirstPersonCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Game/CGameStateBase.h"
#include "Game/CPlayerState.h"
#include "Global.h"
#include "Game/CSpawnPoint.h"
#include "EngineUtils.h"

AFP_FirstPersonGameMode::AFP_FirstPersonGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	// Default Pawn Class
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// HUD Class
	HUDClass = AFP_FirstPersonHUD::StaticClass();

	// GameState Class
	GameStateClass = ACGameStateBase::StaticClass();

	// PlayerState Class
	PlayerStateClass = ACPlayerState::StaticClass();
}

void AFP_FirstPersonGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	CLog::Print("PostLogin Called");

	// Get Player State & Get Player Pawn
	ACPlayerState* playerState = Cast<ACPlayerState>(NewPlayer->PlayerState);
	CheckNull(playerState);

	AFP_FirstPersonCharacter* playerPawn = Cast<AFP_FirstPersonCharacter>(NewPlayer->GetPawn());
	CheckNull(playerPawn);

	playerPawn->SetPlayerState(playerState);

	// �� �й�
	if (RedTeamPlayers.Num() > BlueTeamPlayers.Num())
	{
		BlueTeamPlayers.Add(playerPawn);
		playerState->Team = ETeamType::Blue;
	}
	else
	{
		RedTeamPlayers.Add(playerPawn);
		playerState->Team = ETeamType::Red;
	}

	// �� ����
	playerPawn->CurrentTeam = playerState->Team;
	playerPawn->SetTeamColor(playerState->Team);

	// Get SpawnPoints
	UWorld* world = GetWorld();
	CheckNull(world);

	// iterator�� null�� �� ������
	//for (TActorIterator<ACSpawnPoint> it(world); it; ++it)
	//{
	//	if (it->GetTeam() == ETeamType::Red)
	//		RedTeamSpawnPoints.Add(*it);
	//	else
	//		BlueTeamSpawnPoints.Add(*it);
	//}

	/*CLog::Log("RedTeam SpawnPoints : " + FString::FromInt(RedTeamSpawnPoints.Num()));
	CLog::Log("BlueTeam SpawnPoints : " + FString::FromInt(BlueTeamSpawnPoints.Num()));
	
	CLog::Log("RedTeam : " + FString::FromInt(RedTeamPlayers.Num()));
	CLog::Log("BlueTeam : " + FString::FromInt(BlueTeamPlayers.Num()));*/


	SpawnHost(world);
	MoveToSpawnPoint(playerPawn);
}

void AFP_FirstPersonGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void AFP_FirstPersonGameMode::SpawnHost(UWorld* world)
{
	APlayerController* hostController = world->GetFirstPlayerController();
	CheckNull(hostController);

	AFP_FirstPersonCharacter* hostPawn = Cast<AFP_FirstPersonCharacter>(hostController->GetPawn());
	CheckNull(hostPawn);

	MoveToSpawnPoint(hostPawn);

}

void AFP_FirstPersonGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	// Get SpawnPoints
	UWorld* world = GetWorld();
	CheckNull(world);

	// iterator�� null�� �� ������
	for (TActorIterator<ACSpawnPoint> it(world); it; ++it)
	{
		if (it->GetTeam() == ETeamType::Red)
			RedTeamSpawnPoints.Add(*it);
		else
			BlueTeamSpawnPoints.Add(*it);
	}

	CLog::Log("RedTeam SpawnPoints : " + FString::FromInt(RedTeamSpawnPoints.Num()));
	CLog::Log("BlueTeam SpawnPoints : " + FString::FromInt(BlueTeamSpawnPoints.Num()));

	CLog::Log("RedTeam : " + FString::FromInt(RedTeamPlayers.Num()));
	CLog::Log("BlueTeam : " + FString::FromInt(BlueTeamPlayers.Num()));
}

void AFP_FirstPersonGameMode::MoveToSpawnPoint(AFP_FirstPersonCharacter* InPlayer)
{
	TArray<ACSpawnPoint*>* targetPoints;

	if (InPlayer->CurrentTeam == ETeamType::Red)
		targetPoints = &RedTeamSpawnPoints;
	else
		targetPoints = &BlueTeamSpawnPoints;

	for (const auto& target : *targetPoints)
	{
		// ���� ����Ʈ�� ����Ǿ� ���� ���� ���
		if (target->IsBlocked() == false)
		{
			InPlayer->SetActorLocation(target->GetActorLocation());
			target->UpdateOverlaps();

			return;
		}
	}
	// ���� ����Ʈ�� ����Ǿ� �ִ� ���
	if (WaitingPlayers.Find(InPlayer) < 0)
		WaitingPlayers.Add(InPlayer);
}