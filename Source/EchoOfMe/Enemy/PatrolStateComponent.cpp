// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolStateComponent.h"
#include "EchoOfMe/Enemy/EchoEnemyBehaviorComponent.h"
#include "EchoEnemyAIController.h"
#include "Enemy/EchoEnemy.h"
#include "NavigationSystem.h"


void UPatrolStateComponent::OnStateEnter()
{
	Super::OnStateEnter();

	if (!EchoEnemy) return;
	
	bHasTarget = PickRandomNavMovePoint(RandomPickTarget);

	StuckTime = 0.0f;

	if(bHasTarget)
	{
		const bool bStarted = EnemyBrain->RequestMoveTo(RandomPickTarget);


		if (!bStarted)
		{
			bHasTarget = false;
		}

	}


}

void UPatrolStateComponent::OnStateUpdate(float Delta)
{
	Super::OnStateUpdate(Delta);

	if (!EnemyBrain) return;
	if (!EchoEnemy) return;

	if (EnemyBrain->AttackRadius <= EchoEnemy->GetDistanceToPlayer())
	{

	}

	if (!bHasTarget)
	{
		bHasTarget = PickRandomNavMovePoint(RandomPickTarget);
		const bool bStarted = EnemyBrain->RequestMoveTo(RandomPickTarget);
		return;
	}

	if (!EnemyBrain->IsNavMoving())
	{
		bHasTarget = PickRandomNavMovePoint(RandomPickTarget);
		const bool bStarted = EnemyBrain->RequestMoveTo(RandomPickTarget);
	}

	if (StuckTime >= MaxStuckTime)
	{

		bHasTarget = PickRandomNavMovePoint(RandomPickTarget);
		const bool bStarted = EnemyBrain->RequestMoveTo(RandomPickTarget);

	}


}

void UPatrolStateComponent::OnStateExit()
{
	Super::OnStateExit();





}

bool UPatrolStateComponent::PickRandomNavMovePoint(FVector& OutLocation) const
{
	if (!EchoEnemy) return false;

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(EchoEnemy->GetWorld());
	if (!NavSystem) return false;

	FNavLocation NavLocation;

	const bool bNavFount = NavSystem->GetRandomReachablePointInRadius(EchoEnemy->GetActorLocation(),PatrolRadius, NavLocation);

	if (bNavFount) {
		OutLocation = NavLocation;
	}

	return false;
}
