// Fill out your copyright notice in the Description page of Project Settings.


#include "SearchStateComponent.h"
#include "EchoOfMe/Enemy/EchoEnemyBehaviorComponent.h"

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

	if (WanderHoldTimer <= 0.0)
	{
		EnemyBrain->PickCustomRadiusNavLocation(LastLocation, 3000.0f);
		WanderHoldTimer
	}
	


}

void USearchStateComponent::OnStateExit()
{
	Super::OnStateExit();




}
