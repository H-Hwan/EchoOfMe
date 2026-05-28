// Fill out your copyright notice in the Description page of Project Settings.


#include "SearchStateComponent.h"
#include "EchoOfMe/Enemy/EchoEnemyBehaviorComponent.h"
#include"Enemy/EchoEnemy.h"
void USearchStateComponent::OnStateEnter()
{
	Super::OnStateEnter();

	LastLocation = EnemyBrain->GetPlayerLocation();
	EnemyBrain->RequestMoveTo(LastLocation);



}

void USearchStateComponent::OnStateUpdate(float Delta)
{
	Super::OnStateUpdate(Delta);


	if (EnemyBrain->IsPlayerInDetectedSight())
	{
		UE_LOG(LogTemp, Error, TEXT("[수색중 적발견]"));
		EnemyBrain->ChangeState(EFSMState::Chase);
		return;
	}
	
	if (FVector::Distance(LastLocation, EchoEnemy->GetActorLocation()) > 100.0f)
	{
		return;
	}
	WanderHoldTimer -= Delta;
	if (WanderHoldTimer < 0.0f)
	{
		EnemyBrain->PickCustomRadiusNavLocation(LastLocation, 600.0f);
		EnemyBrain->RequestMoveTo(LastLocation);
		WanderHoldTimer = 5.0f;
	}
	ChangeStateTimer -= Delta;

	if (ChangeStateTimer <= 0.0f)
	{
		EnemyBrain->ChangeState(EFSMState::Lost);
		return;
	}




}

void USearchStateComponent::OnStateExit()
{
	Super::OnStateExit();

	WanderHoldTimer = 5.0f;

	ChangeStateTimer = 10.0f;
}
