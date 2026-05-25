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

// 경로 추적시 이벤트를 받아 처리하고 싶을 때 모듈 포함
#include "Navigation/PathFollowingComponent.h"
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

	CurrentState = EFSMState::Suspect;
}


// Called when the game starts
void UEchoEnemyBehaviorComponent::BeginPlay()
{
	Super::BeginPlay();

	Echo = Cast<AEchoEnemy>(GetOwner());

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

FVector UEchoEnemyBehaviorComponent::GetDistanceToPlayer()
{
	FVector PlayerLoc = GetPlayerInfo()->GetActorLocation();
	FVector Loc = Echo->GetActorLocation();
	FVector DistanceToPlayer = PlayerLoc - Loc;

	UE_LOG(LogTemp, Log, TEXT("[UEchoEnemyBehaviorComponent] %f , %f"),DistanceToPlayer.X, DistanceToPlayer.Y);

	return DistanceToPlayer;
}
FVector UEchoEnemyBehaviorComponent::GetPlayerLocation()
{
	return GetPlayerInfo()->GetActorLocation();
}
// 깃발 배열돌려야됨
void UEchoEnemyBehaviorComponent::PickTeleportToNewPoint()
{
	TArray<AActor*> Flags;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), SelectTag, Flags);

	CachedFlags.Reset();
	for (AActor* A : Flags)
	{
		if (FVector::Dist(A->GetActorLocation(), GetPlayerLocation()) <= OutRange)
		{
			continue;
		}
		CachedFlags.Add(A);
	}

	if (CachedFlags.Num() <= 0) return;

	int32 Random = FMath::RandRange(0, CachedFlags.Num() - 1);

	AActor* ACT = CachedFlags[Random];

	Echo->SetActorLocation(ACT->GetActorLocation());
}



