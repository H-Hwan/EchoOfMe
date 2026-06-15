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
	UE_LOG(LogTemp, Error, TEXT("[UPatrolStateComponent] Patrol Starting"));

	CurrentRandomChangePatrolToAmbush = FMath::RandRange(0, 10);

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
		if (PatrolToAmbush >= CurrentRandomChangePatrolToAmbush)// 지금 스토킹 깃발로 이동
		{
			UE_LOG(LogTemp, Error, TEXT("[스토킹 시작]"));

			EnemyBrain->ChangeState(EFSMState::Ambush);
			return;
		}
		UE_LOG(LogTemp, Error, TEXT("[추격 시작]"));
		EnemyBrain->ChangeState(EFSMState::Chase);
		return;
	}

	if (EnemyBrain->IsLightDetected())
	{
		UE_LOG(LogTemp, Error, TEXT("[빛을 봤잖아]"));
		EnemyBrain->ChangeState(EFSMState::Suspect);
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

	//if (!EnemyBrain->IsNavMoving() && EnemyBrain->GetDistanceToPlayer() >= 30.0f && bNeedNewTarget == false)
	//{
	//	EnemyBrain->PickRandomNavMovePoint(RandomPickTarget);
	//	EnemyBrain->RequestMoveTo(RandomPickTarget);
	//}

	TargetRetryTimer -= Delta;
	if (bNeedNewTarget && TargetRetryTimer <= 0.0f)
	{
		if (EnemyBrain->PickRandomNavMovePoint(RandomPickTarget))
		{
			const bool bStarted = EnemyBrain->RequestMoveTo(RandomPickTarget);
			bHasTarget = bStarted;
			UE_LOG(LogTemp, Warning, TEXT("[Echo Patro] 움직임 "));
			if (bHasTarget)
			{
				StuckTime = 0.0f;
			}
			TargetRetryTimer = 2.0f;
			// 움직임 여부
			bIsNowMove = 1;
		}
		else
		{
			bHasTarget = false;
			TargetRetryTimer = 2.0f;
			UE_LOG(LogTemp, Warning, TEXT("[Echo Patrol] 타겟 찾지 못함"));
		}
	}

	if (LookingTime > 0.0f && !EnemyBrain->IsNavMoving())
	{
		LookingTime -= Delta;
		LookingForwardTime -= Delta;

		// 1. 목표 각도(TargetRotation) 갱신: 2초마다 한 번씩 실행
		if (LookingForwardTime <= 0.0f)
		{
			FRotator CurrentRot = EchoEnemy->GetActorRotation();

			// 도리도리 각도
			float RandomYawOffset = FMath::RandRange(LookingAngle * -1, LookingAngle);

			RotationToTarget = CurrentRot;
			// 핵심: 기존 각도에 랜덤 오프셋을 '더해줍니다'
			RotationToTarget.Yaw += RandomYawOffset;

			LookingForwardTime = 2.0f; // 타이머 리셋
		}

		// 2. 실제 회전 적용: 타이머 안쪽이 아니라 바깥으로 빼서 **매 프레임(틱)마다** 실행되게 합니다.
		FRotator SmoothRot = FMath::RInterpTo(EchoEnemy->GetActorRotation(), RotationToTarget, Delta, 3.0f);
		EchoEnemy->SetActorRotation(SmoothRot);
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
