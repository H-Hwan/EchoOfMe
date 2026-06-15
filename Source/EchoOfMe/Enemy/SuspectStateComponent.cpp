// Fill out your copyright notice in the Description page of Project Settings.


#include "SuspectStateComponent.h"
#include "EchoOfMe/Enemy/EchoEnemyBehaviorComponent.h"
#include "Enemy/EchoEnemy.h"
#include "NavigationSystem.h"
void USuspectStateComponent::OnStateEnter()
{
	Super::OnStateEnter();

	bLooking = false;
	LookingTime = 8.0f;
	LookingForwardTime = 2.0f;
	RotationToTarget = FRotator::ZeroRotator;
	UE_LOG(LogTemp, Log, TEXT("[의심 시작]"));

	CurrentRandomChangeSuspectToAmbush = FMath::RandRange(0, 10);
}

void USuspectStateComponent::OnStateUpdate(float Delta)
{
	Super::OnStateUpdate(Delta);

	if (EnemyBrain->IsPlayerInDetectedSight())
	{
		if (SuspectToAmbush >= CurrentRandomChangeSuspectToAmbush)// 지금 스토킹 포인트 이동
		{
			UE_LOG(LogTemp, Error, TEXT("[스토킹 시작]"));
			EnemyBrain->ChangeState(EFSMState::Ambush);
			return;
		}

		EnemyBrain->ChangeState(EFSMState::Chase);
		UE_LOG(LogTemp, Log, TEXT("[의심 시작] 체이스 시작"));
		return;
	}
	if (EnemyBrain->IsLightDetected())
	{
		FVector CurrentLightLoc = EnemyBrain->IsLightLoc();

		// 스팸 방지: 마우스 떨림을 무시하도록 거리를 150.0f로 약간 늘림
		if (FVector::Dist(EnemyBrain->LastDestination, CurrentLightLoc) > 150.0f)
		{
			// ★ 핵심 보정 1: 벽이나 허공에 찍힌 빛의 좌표를 수직 아래 바닥(NavMesh)으로 끌어내립니다.
			UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
			FNavLocation ProjectedLoc;

			// 반경 500 단위 내에서 가장 가까운 바닥을 찾습니다.
			if (NavSystem && NavSystem->ProjectPointToNavigation(CurrentLightLoc, ProjectedLoc, FVector(500.0f, 500.0f, 500.0f)))
			{
				EnemyBrain->RequestMoveTo(ProjectedLoc.Location);
			}
			else
			{
				// 바닥을 못 찾았을 경우 일단 원본 좌표로 이동 시도
				EnemyBrain->RequestMoveTo(CurrentLightLoc);
			}
		}

		// ★ 핵심 보정 2: AI가 굳어버리는 현상 해결
		// 길찾기에 성공해서 AI가 '실제로 이동 중(NavMoving)'일 때만 return으로 하위 로직을 차단합니다.
		// 만약 길이 없어 멈췄거나, 빛 근처에 도착했다면 return하지 않고 자연스럽게 아래의 '두리번거리기' 로직으로 넘어갑니다.
		if (EnemyBrain->IsNavMoving())
		{
			return;
		}
	}


	if (LookingTime > 0.0f)
	{
		LookingTime -= Delta;

		if (LookingForwardTime >= 0.0f)
		{
			LookingForwardTime -= Delta;
		}
		else
		{
			FRotator CurrentRot = EchoEnemy->GetActorRotation();

			int32 Random = FMath::RandRange(-45, 45);

			RotationToTarget = CurrentRot;

			RotationToTarget.Yaw = Random;

			EchoEnemy->SetActorRotation(FMath::RInterpTo(EchoEnemy->GetActorRotation(), RotationToTarget, Delta, 5.0f));

			LookingForwardTime = 2.0f;
		}
	}
	else
	{
		EnemyBrain->ChangeState(EFSMState::Patrol);
		return;
	}



}

void USuspectStateComponent::OnStateExit()
{
	Super::OnStateExit();

	LookingTime = 8.0f;

	LookingForwardTime = 2.0f;

	bLooking = false;

	RotationToTarget = FRotator::ZeroRotator;
}
