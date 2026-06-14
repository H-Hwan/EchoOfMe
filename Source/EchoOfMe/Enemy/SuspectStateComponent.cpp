// Fill out your copyright notice in the Description page of Project Settings.


#include "SuspectStateComponent.h"
#include "EchoOfMe/Enemy/EchoEnemyBehaviorComponent.h"
#include "Enemy/EchoEnemy.h"

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
		UE_LOG(LogTemp, Error, TEXT("[[Suspect]빛을 봤잖아]"));
		EnemyBrain->RequestMoveTo(EnemyBrain->IsLightLoc());
		return;
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
