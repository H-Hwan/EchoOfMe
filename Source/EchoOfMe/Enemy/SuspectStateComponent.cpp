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


}

void USuspectStateComponent::OnStateUpdate(float Delta)
{
	Super::OnStateUpdate(Delta);

	if (EnemyBrain->DetectedMinimumDistanceRadius >= EnemyBrain->GetDistanceToPlayer() || EnemyBrain->IsPlayerInDetectedSight())
	{
		EnemyBrain->ChangeState(EFSMState::Chase);
		return;
	}

	if (LookingTime > 0.0f)
	{
		LookingTime -= Delta;
	}
	else
	{
		EnemyBrain->ChangeState(EFSMState::Patrol);
		return;
	}

	if (LookingForwardTime >= 0.0f)
	{
		LookingForwardTime -= Delta;
	}
	else
	{
		FVector Forward = EchoEnemy->GetActorForwardVector();



		LookingForwardTime = 2.0f;
	}


}

void USuspectStateComponent::OnStateExit()
{
	Super::OnStateExit();

	LookingTime = 8.0f;

	LookingForwardTime = 2.0f;

	bLooking = false;

}
