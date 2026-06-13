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
#include "Enemy/ResonanceSensorComponent.h" /// 감지 센서
#include "Navigation/PathFollowingComponent.h"
#include "DrawDebugHelpers.h"
#include "Enemy/AmbushStateComponent.h"
#include "Component/FlashlightComponent.h"

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
	AmbushStateComp = CreateDefaultSubobject<UAmbushStateComponent>(TEXT("AmbushState"));

	CurrentSensorValue = MaxSensorValue;
}


// Called when the game starts
void UEchoEnemyBehaviorComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedPlayer = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (CachedPlayer)
	{
		FlashlightComponent = CachedPlayer->FindComponentByClass<UFlashlightComponent>();
	}
	Echo = Cast<AEchoEnemy>(GetOwner());
	CurrentState = EFSMState::Patrol;
	PatrolStateComp->Startreference(Echo, this);
	SuspectStateComp->Startreference(Echo, this);
	ChaseStateComp->Startreference(Echo, this);
	SearchStateComp->Startreference(Echo, this);
	LostStateComp->Startreference(Echo, this);
	AmbushStateComp->Startreference(Echo, this);

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
	CheckIfStuck(DeltaTime);
}
// 상태반환메소드
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
		return AmbushStateComp;
	default:
		return nullptr;
	}
}
// 상태 변환 메소드
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
// 지정된 위치로 이동
bool UEchoEnemyBehaviorComponent::RequestMoveTo(const FVector& Destination, float InAcceptanceRadius)
{
	AAIController* AIController = Cast<AAIController>(Echo->GetController());
	LastDestination = Destination;
	if (!AIController) return false;

	const float Radius = (InAcceptanceRadius > 0.0f) ? InAcceptanceRadius : AcceptanceRadius;

	FAIMoveRequest Moveq;

	Moveq.SetGoalLocation(Destination);
    Moveq.SetAcceptanceRadius(Radius);
    Moveq.SetAllowPartialPath(false);  // ✅ false로 변경
    Moveq.SetUsePathfinding(true);
    Moveq.SetProjectGoalLocation(true);

	const FPathFollowingRequestResult Result = AIController->MoveTo(Moveq);

	return Result.Code != EPathFollowingRequestResult::Failed;
}
// 플레이어 정보 반환
APawn* UEchoEnemyBehaviorComponent::GetPlayerInfo() const
{
	return CachedPlayer;
}
// 플레이어의 위치값 반환
FVector UEchoEnemyBehaviorComponent::GetPlayerLocation()
{
	if (!CachedPlayer)
		return FVector::ZeroVector;

	return CachedPlayer->GetActorLocation();
}
// 플레이어가 시야 내로 들어섰는가
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

bool UEchoEnemyBehaviorComponent::IsTargetInSight(const FVector& TargetLocation)
{
	FVector EnemyLocation = Echo->GetActorLocation();

	float Distance = FVector::Distance(EnemyLocation, TargetLocation);

	if (Distance > MaxDistance) return false;

	FVector ForwardV = Echo->GetActorForwardVector();

	FVector ToTarget = (TargetLocation - EnemyLocation).GetSafeNormal();

	float DotPro = FVector::DotProduct(ForwardV, ToTarget);

	CosAngle = FMath::Cos(FMath::DegreesToRadians(MaxDegreeLimit));

	return DotPro > CosAngle;
}
bool UEchoEnemyBehaviorComponent::IsLightDetected()
{
	return IsTargetInSight(FlashlightComponent->LightEndPoint());
}
// 깃발을 배열로 돌린후 플레이어으로부터 일정 거리 초과 범위내 랜덤 텔레포팅
FVector UEchoEnemyBehaviorComponent::PickTeleportToNewPoint()
{

	TArray<AActor*> Flags;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), SelectTag, Flags);
	if (SelectTag == NAME_None) return FVector::ZeroVector;

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

	UE_LOG(LogTemp, Log, TEXT("[PickTeleportToNewPoint] 스폰 지점 순간이동"));

	return ACT->GetActorLocation();
}
// Enemy 신체를 반만 노출할 지점 지정
FVector UEchoEnemyBehaviorComponent::FindPeekPoint()
{

	TArray<AActor*> PeekPoints;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(),PeekPointName, PeekPoints);

	AActor* ClosestPoint = nullptr;
	float MinDistanceToEnemySq = MAX_FLT;
	FVector EnemyLocation = Echo->GetActorLocation();
	for (AActor* PP : PeekPoints)
	{
		float DistToPlayer = FVector::Dist2D(PP->GetActorLocation(), GetPlayerLocation());
		if (DistToPlayer >= 700.0f && DistToPlayer <= 2000.0f)
		{
			// 2. 적(자신)과의 거리 계산 
			// (단순 크기 비교용이므로 연산이 무거운 루트계산(Dist) 대신 제곱계산(DistSquared)을 사용하여 최적화)
			float DistToEnemySq = FVector::DistSquared(PP->GetActorLocation(), EnemyLocation);

			// 3. 지금까지 찾은 포인트보다 더 가깝다면 갱신
			if (DistToEnemySq < MinDistanceToEnemySq)
			{
				MinDistanceToEnemySq = DistToEnemySq;
				ClosestPoint = PP;
			}
		}

	}
	if (ClosestPoint != nullptr)
	{
		return ClosestPoint->GetActorLocation();
	}
	return FVector::ZeroVector;
}

// 움직임이 있는가
bool UEchoEnemyBehaviorComponent::IsNavMoving() const
{
	if (!Echo) return false;

	
	AAIController* AIController = Cast<AAIController>(Echo->GetController());
	if (!AIController) return false;


	return AIController->GetMoveStatus() != EPathFollowingStatus::Idle;
}
// 배회 지점 2
bool UEchoEnemyBehaviorComponent::PickCustomRadiusNavLocation(FVector& OutLocation,float Radius)
{

	if (!Echo) return false;
	UE_LOG(LogTemp, Warning, TEXT("[PickCustomRadius] 입력받은 반경: %f, 에너미 위치: %s"), Radius, *Echo->GetActorLocation().ToString());


	DrawDebugSphere(GetWorld(), Echo->GetActorLocation(), Radius, 32, FColor::Turquoise , false, 2.0f);
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(Echo->GetWorld());
	if (!NavSystem) return false;

	FNavLocation NavLocation;

	const bool bNavFount = NavSystem->GetRandomReachablePointInRadius(Echo->GetActorLocation(), Radius, NavLocation);

	if (bNavFount) {
		OutLocation = NavLocation.Location;
		DrawDebugPoint(GetWorld(), OutLocation, 20.0f, FColor::Red, false, 2.0f);
		UE_LOG(LogTemp, Log, TEXT("[PickCustomRadius] 성공! 찾은 목적지: %s"), *OutLocation.ToString());
	}

	return bNavFount;
}
// 플레이어와의 거리
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

//랜덤좌표 뽑기
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
// 플레이어를 놓쳤는가
bool UEchoEnemyBehaviorComponent::IsPlayerLoseInSight()
{
	return GetDistanceToPlayer() > LoseDistance;
}

void UEchoEnemyBehaviorComponent::SetSoundSensorValue(bool bSound, int32 Value)
{


}

void UEchoEnemyBehaviorComponent::SetLightSensorValue(bool bLight, int32 Value)
{



}

void UEchoEnemyBehaviorComponent::SetResonanceSensorValue(bool bResonance, int32 Value)
{



}

void UEchoEnemyBehaviorComponent::CheckIfStuck(float DeltaTime)
{
	if (!IsNavMoving())
	{
		StuckTimer = 0.0f;
		LastPosition = Echo->GetActorLocation();
		return;
	}

	StuckTimer += DeltaTime;

	if (StuckTimer >= StuckCheckInterval)
	{
		StuckTimer = 0.0f;
		float MovedDistance = FVector::Dist(Echo->GetActorLocation(), LastPosition);

		if (MovedDistance < StuckThreshold)
		{
			UE_LOG(LogTemp, Warning, TEXT("[CheckIfStuck] 막힘 감지 → 우회 시도"));

			
			FVector ToDestination = (LastDestination - Echo->GetActorLocation()).GetSafeNormal();
			FVector RightVector = FVector::CrossProduct(ToDestination, FVector::UpVector);

			float Side = (FMath::RandBool()) ? 1.0f : -1.0f;
			FVector DetourPoint = Echo->GetActorLocation() + (RightVector * Side * 200.0f);
	

	
			RequestMoveToInternal(DetourPoint);
			RequestMoveTo(DetourPoint);  // 우회 지점으로 먼저 이동
		}

		LastPosition = Echo->GetActorLocation();
	}
}

bool UEchoEnemyBehaviorComponent::RequestMoveToInternal(const FVector& Destination)
{
	AAIController* AIController = Cast<AAIController>(Echo->GetController());
	if (!AIController) return false;

	FAIMoveRequest Moveq;
	Moveq.SetGoalLocation(Destination);
	Moveq.SetAcceptanceRadius(AcceptanceRadius);
	Moveq.SetAllowPartialPath(false);
	Moveq.SetUsePathfinding(true);
	Moveq.SetProjectGoalLocation(true);

	const FPathFollowingRequestResult Result = AIController->MoveTo(Moveq);
	return Result.Code != EPathFollowingRequestResult::Failed;
	//-----------------------------
	AAIController* AIController = Cast<AAIController>(Echo->GetController());
	LastDestination = Destination;
	if (!AIController) return false;

	const float Radius = (InAcceptanceRadius > 0.0f) ? InAcceptanceRadius : AcceptanceRadius;

	FAIMoveRequest Moveq;

	Moveq.SetGoalLocation(Destination);
	Moveq.SetAcceptanceRadius(Radius);
	Moveq.SetAllowPartialPath(false);  // ✅ false로 변경
	Moveq.SetUsePathfinding(true);
	Moveq.SetProjectGoalLocation(true);

	const FPathFollowingRequestResult Result = AIController->MoveTo(Moveq);

	return Result.Code != EPathFollowingRequestResult::Failed;
}
