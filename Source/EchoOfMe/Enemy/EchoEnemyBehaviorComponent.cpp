// Fill out your copyright notice in the Description page of Project Settings.


#include "EchoEnemyBehaviorComponent.h"
#include "EchoOfMe/Enemy/PatrolStateComponent.h"
#include "EchoOfMe/Enemy/SuspectStateComponent.h"
#include "EchoOfMe/Enemy/ChaseStateComponent.h"
#include "EchoOfMe/Enemy/SearchStateComponent.h"
#include "EchoOfMe/Enemy/LostStateComponent.h"
#include "EchoOfMe/Enemy/EchoEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "EchoOfMe/Enemy/EchoEnemyAIController.h"
#include "NavigationSystem.h"
#include "ResonanceSensorComponent.h" /// 감지 센서
#include "Navigation/PathFollowingComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UEchoEnemyBehaviorComponent::UEchoEnemyBehaviorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	PatrolStateComp = CreateDefaultSubobject<UPatrolStateComponent>(TEXT("PatrolState"));
	SuspectStateComp = CreateDefaultSubobject<USuspectStateComponent>(TEXT("SuspectState"));
	ChaseStateComp = CreateDefaultSubobject<UChaseStateComponent>(TEXT("ChaseState"));
	SearchStateComp = CreateDefaultSubobject<USearchStateComponent>(TEXT("SearchState"));
	LostStateComp = CreateDefaultSubobject<ULostStateComponent>(TEXT("LostState"));


}


// Called when the game starts
void UEchoEnemyBehaviorComponent::BeginPlay()
{
	Super::BeginPlay();

	Echo = Cast<AEchoEnemy>(GetOwner());
	CurrentState = EFSMState::Patrol;
	PatrolStateComp->Startreference(Echo, this);
	SuspectStateComp->Startreference(Echo, this);
	ChaseStateComp->Startreference(Echo, this);
	SearchStateComp->Startreference(Echo, this);
	LostStateComp->Startreference(Echo, this);

	CurrentStateComp = GetStateComponent(CurrentState);
	if (CurrentStateComp)
	{
		CurrentStateComp->OnStateEnter();
	}

}


// Called every frame
void UEchoEnemyBehaviorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (CurrentStateComp) {
	
		CurrentStateComp->OnStateUpdate(DeltaTime);
	}
	
}

UFSMStateBase* UEchoEnemyBehaviorComponent::GetStateComponent(EFSMState NewState)
{
	switch (NewState)
	{
	case EFSMState::Patrol:
		return PatrolStateComp;
	case EFSMState::Suspect:
		return SuspectStateComp;
	case EFSMState::Chase:
		return ChaseStateComp;
	case EFSMState::Search:
		return SearchStateComp;
	case EFSMState::Lost:
		return LostStateComp;
	case EFSMState::Ambush:
		return nullptr;
	default:
		return nullptr;
	}
}

void UEchoEnemyBehaviorComponent::ChangeState(EFSMState NewState)
{
	if (CurrentState == NewState || CurrentStateComp == nullptr) return;

	if (CurrentStateComp)
	{
		CurrentStateComp->OnStateExit();
	}

	CurrentState = NewState;
	CurrentStateComp = GetStateComponent(NewState);

	if (CurrentStateComp)
	{
		CurrentStateComp->OnStateEnter();
	}

}

bool UEchoEnemyBehaviorComponent::RequestMoveTo(const FVector& Destination, float InAcceptanceRadius)
{
	AAIController* AIController = Cast<AAIController>(Echo->GetController());

	if (!AIController) return false;

	const float Radius = (InAcceptanceRadius > 0.0f) ? InAcceptanceRadius : AcceptanceRadius;

	FAIMoveRequest Moveq;

	Moveq.SetGoalLocation(Destination);
	Moveq.SetAcceptanceRadius(Radius);
	Moveq.SetAllowPartialPath(true);
	Moveq.SetUsePathfinding(true);
	Moveq.SetProjectGoalLocation(true);

	const FPathFollowingRequestResult Result = AIController->MoveTo(Moveq);



	return Result.Code != EPathFollowingRequestResult::Failed;
}

APawn* UEchoEnemyBehaviorComponent::GetPlayerInfo() const
{
	return UGameplayStatics::GetPlayerPawn(GetWorld(),0); 
}

FVector UEchoEnemyBehaviorComponent::GetPlayerLocation()
{

	return GetPlayerInfo()->GetActorLocation();
}

bool UEchoEnemyBehaviorComponent::IsPlayerInDetectedSight()
{
	if (!Echo || !GetPlayerInfo()) return false;

	FVector EnemyLocation = Echo->GetActorLocation();

	FVector PlayerLocation = GetPlayerLocation();

	float Distance = FVector::Distance(EnemyLocation, PlayerLocation);

	if (Distance > MaxDistance) return false;

	FVector ForwardV = Echo->GetActorForwardVector();

	FVector TDV = (PlayerLocation - EnemyLocation).GetSafeNormal();

	float DotResult = FVector::DotProduct(ForwardV, TDV);

	CosAngle = FMath::Cos(FMath::DegreesToRadians(MaxDegreeLimit));

	if (DotResult <= CosAngle) return false;

	FHitResult HitResult;

	FCollisionQueryParams Param;

	Param.AddIgnoredActor(Echo);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, EnemyLocation + FVector(0.0f, 0.0f, 60.0f), PlayerLocation + FVector(0.0f, 0.0f, 60.0f), ECC_Pawn, Param);
	
	if (bHit && HitResult.GetActor() == GetPlayerInfo())
	{
		DrawDebugLine(GetWorld(), EnemyLocation + FVector(0.0f, 0.0f, 60.0f), PlayerLocation + FVector(0.0f, 0.0f, 60.0f), FColor::Purple, false, -1.0f, 0, 2.0f);
		return true;
	}

	return false;
}

// 깃발 배열돌려야됨 깃발 위치로 순간이동
FVector UEchoEnemyBehaviorComponent::PickTeleportToNewPoint()
{
	TArray<AActor*> Flags;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), SelectTag, Flags);

	CachedFlags.Reset();
	for (AActor* A : Flags)
	{
		if (FVector::Dist(A->GetActorLocation(), GetPlayerLocation()) <= SpawnMinimumDistance)
		{

			continue;
		}
		CachedFlags.Add(A);
	}

	if (CachedFlags.Num() <= 0) return FVector::ZeroVector;

	int32 Random = FMath::RandRange(0, CachedFlags.Num() - 1);
	// 액터
	AActor* ACT = CachedFlags[Random];

	return ACT->GetActorLocation();
}


bool UEchoEnemyBehaviorComponent::IsNavMoving() const
{
	if (!Echo) return false;

	// 주인의 AI 컨트롤러를 가져옵니다.
	AAIController* AIController = Cast<AAIController>(Echo->GetController());
	if (!AIController) return false;

	// ⭐️ 완벽한 정답 복구: Idle(대기/멈춤) 상태가 아니라면 이동 중이거나 길을 찾는 중이라는 뜻!
	return AIController->GetMoveStatus() != EPathFollowingStatus::Idle;
}

bool UEchoEnemyBehaviorComponent::PickCustomRadiusNavLocation(FVector& OutLocation,float Radius)
{

	if (!Echo) return false;
	UE_LOG(LogTemp, Warning, TEXT("[PickCustomRadius] 입력받은 반경: %f, 에너미 위치: %s"), Radius, *Echo->GetActorLocation().ToString());

	// ⭐️ [디버그 2] 에너미 발밑에 Radius 크기만큼의 탐색 범위를 파란색 구체로 그려서 눈으로 확인합니다!
	DrawDebugSphere(GetWorld(), Echo->GetActorLocation(), Radius, 32, FColor::Blue, false, 2.0f);
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(Echo->GetWorld());
	if (!NavSystem) return false;

	FNavLocation NavLocation;

	const bool bNavFount = NavSystem->GetRandomPointInNavigableRadius(Echo->GetActorLocation(), Radius, NavLocation);

	if (bNavFount) {
		OutLocation = NavLocation.Location;
		DrawDebugPoint(GetWorld(), OutLocation, 20.0f, FColor::Red, false, 2.0f);
		UE_LOG(LogTemp, Log, TEXT("[PickCustomRadius] 성공! 찾은 목적지: %s"), *OutLocation.ToString());
	}

	return bNavFount;
}

float UEchoEnemyBehaviorComponent::GetDistanceToPlayer() const
{
	if (const APawn* Player = GetPlayerInfo())
	{
		// 두 위치(플레이어, AI캐릭터)간의 거리를 측정함
		return FVector::Dist(Echo->GetActorLocation(), Player->GetActorLocation());
	}

	// TNumericLimits<float>::Min() : 실수에서 가장 작은(무한대)값을 반환
	// TNumericLimits<float>::Max() : 실수에서 가장 큰(무한대)값을 반환

	return TNumericLimits<float>::Max();
}

bool UEchoEnemyBehaviorComponent::PickRandomNavMovePoint(FVector& OutLocation) const
{

	if (!Echo) return false;

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(Echo->GetWorld());
	if (!NavSystem) return false;

	FNavLocation NavLocation;

	const bool bNavFount = NavSystem->GetRandomPointInNavigableRadius(Echo->GetActorLocation(), PatrolRadius, NavLocation);

	if (bNavFount) {
		OutLocation = NavLocation.Location;
	}

	return bNavFount;
}

bool UEchoEnemyBehaviorComponent::IsPlayerLoseInSight()
{
	return GetDistanceToPlayer() > LoseDistance;
}
