// Fill out your copyright notice in the Description page of Project Settings.


#include "SuspectStateComponent.h"
#include "EchoOfMe/Enemy/EchoEnemyBehaviorComponent.h"
#include "Enemy/EchoEnemy.h"

void USuspectStateComponent::OnStateEnter()
{
	Super::OnStateEnter();




}

void USuspectStateComponent::OnStateUpdate(float Delta)
{
	Super::OnStateUpdate(Delta);

	if (EnemyBrain->DetectedMinimumDistanceRadius >= EnemyBrain->GetDistanceToPlayer() || EnemyBrain->IsPlayerInDetectedSight())
	{
		EnemyBrain->ChangeState(EFSMState::Chase);
		return;
	}


}

void USuspectStateComponent::OnStateExit()
{
	Super::OnStateExit();




}
