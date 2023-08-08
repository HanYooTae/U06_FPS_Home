#include "CMovingPlatform.h"
#include "Global.h"
#include "Materials/MaterialInstanceConstant.h"

ACMovingPlatform::ACMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	//ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset(L"StaticMesh'/Game/Geometry/Meshes/1M_Cube_Chamfer.1M_Cube_Chamfer'");
	//if(meshAsset.Succeeded())
	//	GetStaticMeshComponent()->SetStaticMesh(meshAsset.Object);

	UStaticMesh* meshAsset;
	CHelpers::GetAsset<UStaticMesh>(&meshAsset, "StaticMesh'/Game/Geometry/Meshes/1M_Cube_Chamfer.1M_Cube_Chamfer'");
	GetStaticMeshComponent()->SetStaticMesh(meshAsset);
	GetStaticMeshComponent()->SetRelativeScale3D(FVector(1.f, 1.f, 0.2f));

	UMaterialInstanceConstant* materialAsset;
	CHelpers::GetAsset(&materialAsset, "MaterialInstanceConstant'/Game/PlatForms/MAT_MovingPlatForm_Inst.MAT_MovingPlatForm_Inst'");
	GetStaticMeshComponent()->SetMaterial(0, materialAsset);

	GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
}

void ACMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	// ������ ��ġ ���� Replicate
	if (HasAuthority() == true)
	{
		SetReplicates(true);
		SetReplicateMovement(true);
	}

	StartLocation = GetActorLocation();

	GlobalTargetLocation = GetTransform().TransformPosition(TargetLocation);			//startLocation + TargetLocation
}

void ACMovingPlatform::IncreaseActive()
{
	Active++;
}

void ACMovingPlatform::DecreaseActive()
{
	if(Active > 0)
		Active--;
}

void ACMovingPlatform::Tick(float DeltaTime)
{
	// ��ġ�̵�
	Super::Tick(DeltaTime);
	
	// Active�� 1 �̻��� ��쿡��
	CheckFalse(Active > 0);

	// ���������� �̵�
	if (HasAuthority() == true)		//GetLocalRole() == ENetRole::ROLE_Authority
	{
		// 1. ���� ���� ��ġ ������
		FVector location = GetActorLocation();

		// 2. ���� ���ϱ�
		FVector direction = (GlobalTargetLocation - StartLocation).GetSafeNormal();

		float totalDistance = (StartLocation - GlobalTargetLocation).Size();
		float currentDistance = (location - StartLocation).Size();

		if (currentDistance >= totalDistance)
		{
			//direction = (StartLocation - GlobalTargetLocation).GetSafeNormal();
			FVector temp = StartLocation;
			StartLocation = GlobalTargetLocation;
			GlobalTargetLocation = temp;
		}

		// 2. ��ġ ���
		location += direction * Speed * DeltaTime;

		// 3. ��ġ ����
		SetActorLocation(location);
	}
}
