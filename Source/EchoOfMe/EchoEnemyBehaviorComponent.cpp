// Fill out your copyright notice in the Description page of Project Settings.


#include "EchoEnemyBehaviorComponent.h"
#include "PatrolStateComponent.h"
#include "SuspectStateComponent.h"
#include "ChaseStateComponent.h"
#include "SearchStateComponent.h"
#include "LostStateComponent.h"
#include "EchoOfMe/Enemy/EchoEnemy.h"

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

	PatrolStateComp->Startreference(Echo, this);
	SuspectStateComp->Startreference(Echo, this);
	ChaseStateComp->Startreference(Echo, this);
	SearchStateComp->Startreference(Echo, this);
	LostStateComp->Startreference(Echo, this);

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


