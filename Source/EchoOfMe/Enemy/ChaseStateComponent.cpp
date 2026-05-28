// Fill out your copyright notice in the Description page of Project Settings.


#include "ChaseStateComponent.h"
#include "EchoOfMe/Enemy/EchoEnemyBehaviorComponent.h"
#include "Enemy/EchoEnemy.h"

void UChaseStateComponent::OnStateEnter()
{
	Super::OnStateEnter();

	UE_LOG(LogTemp, Warning, TEXT("[추격]나왜여기임"));


	EchoEnemy->IsLockOnToTarget(true);

}

void UChaseStateComponent::OnStateUpdate(float Delta)
{
	Super::OnStateUpdate(Delta);
	EnemyBrain->IsPlayerInDetectedSight();
	if (EnemyBrain->IsPlayerLoseInSight() && !EnemyBrain->IsPlayerInDetectedSight())
	{
	
		EnemyBrain->ChangeState(EFSMState::Search);
		return;
	}

	EnemyBrain->RequestMoveTo(EnemyBrain->GetPlayerLocation());

}

void UChaseStateComponent::OnStateExit()
{
	Super::OnStateExit();

	EchoEnemy->IsLockOnToTarget(false);




}
