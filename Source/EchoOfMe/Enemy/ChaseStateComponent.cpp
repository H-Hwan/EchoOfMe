// Fill out your copyright notice in the Description page of Project Settings.


#include "ChaseStateComponent.h"
#include "EchoOfMe/Enemy/EchoEnemyBehaviorComponent.h"
#include "Enemy/EchoEnemy.h"

void UChaseStateComponent::OnStateEnter()
{
	Super::OnStateEnter();

	UE_LOG(LogTemp, Warning, TEXT("[추격]"));


	EchoEnemy->IsLockOnToTarget(true);
	CurrentTime = MaxLoseTime;
}

void UChaseStateComponent::OnStateUpdate(float Delta)
{
	Super::OnStateUpdate(Delta);
	EnemyBrain->IsPlayerInDetectedSight();


	if (EnemyBrain->IsPlayerLoseInSight() && !EnemyBrain->IsPlayerInDetectedSight())
	{
		CurrentTime -= Delta;
	}
	
	if (EnemyBrain->IsPlayerLoseInSight() && !EnemyBrain->IsPlayerInDetectedSight() && CurrentTime <= 0.0f)
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

	UE_LOG(LogTemp, Warning, TEXT("[추격] 끝 "));

	CurrentTime = MaxLoseTime;
}
