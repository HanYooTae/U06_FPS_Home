#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CGameStateBase.generated.h"

UENUM(BlueprintType)
enum class ETeamType : uint8
{
	Red,
	Blue,
	Max
};

UCLASS()
class U05_SESSION_API ACGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
};
