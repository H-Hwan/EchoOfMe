// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolStateComponent.h"
#include "EchoOfMe/Enemy/EchoEnemyBehaviorComponent.h"
#include "EchoEnemyAIController.h"
#include "Enemy/EchoEnemy.h"



void UPatrolStateComponent::OnStateEnter()
{
	Super::OnStateEnter();

	if (!EchoEnemy) return;

	bHasTarget = EnemyBrain->PickRandomNavMovePoint(RandomPickTarget);
	TargetRetryTimer = 2.0f;
	StuckTime = 0.0f;
	UE_LOG(LogTemp, Error, TEXT("[UPatrolStateComponent]"));
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

	if (EnemyBrain->IsPlayerInDetectedSight())
	{
		UE_LOG(LogTemp, Error, TEXT("[첫비교]"));
		EnemyBrain->ChangeState(EFSMState::Chase);
		return;
	}

	StuckTime += Delta;
	Suspectmin += Delta;

	bool bNeedNewTarget = false;

	if (EchoEnemy->DetectCurrentCount > EchoEnemy->DetectMaxCount)
	{
		UE_LOG(LogTemp, Error, TEXT("[서칭]"));
		EnemyBrain->ChangeState(EFSMState::Search);
	}
	else if (!bHasTarget) // 목적지가 아예 없거나
	{
		bNeedNewTarget = true;
	}
	else if (StuckTime >= MaxStuckTime) // 어딘가 구석에 끼여서 못 움직인 지 오래됐거나
	{
		bNeedNewTarget = true;
	}
	else if (Suspectmax <= Suspectmin)
	{
		EnemyBrain->ChangeState(EFSMState::Suspect);
		return;
	}
	else if (!EnemyBrain->IsNavMoving()) // 목적지는 있는데 발이 멈췄거나
	{
		bNeedNewTarget = true;
	}

	if (!EnemyBrain->IsNavMoving() && EnemyBrain->GetDistanceToPlayer() >= 30.0f && bNeedNewTarget == false)
	{
		EnemyBrain->PickRandomNavMovePoint(RandomPickTarget);
		EnemyBrain->RequestMoveTo(RandomPickTarget);
	}

	TargetRetryTimer -= Delta;
	if (bNeedNewTarget && TargetRetryTimer <= 0.0f)
	{
		if (EnemyBrain->PickRandomNavMovePoint(RandomPickTarget))
		{
			const bool bStarted = EnemyBrain->RequestMoveTo(RandomPickTarget);
			bHasTarget = bStarted;
			UE_LOG(LogTemp, Warning, TEXT("[Echo Patrol] 타겟 찻았는데 왜 움직이질 못함"));
			if (bHasTarget)
			{
				StuckTime = 0.0f;
			}
			TargetRetryTimer = 2.0f;
		}
		else
		{
			bHasTarget = false;
			TargetRetryTimer = 2.0f;
			UE_LOG(LogTemp, Warning, TEXT("[Echo Patrol] 타겟 찾지 못함"));
		}
	}

}

void UPatrolStateComponent::OnStateExit()
{
	Super::OnStateExit();

	bHasTarget = false;

	RandomPickTarget = FVector::ZeroVector;

	StuckTime = 0.0f;

	Suspectmin = 0.0f;

	TargetRetryTimer = 0.0f;
}
