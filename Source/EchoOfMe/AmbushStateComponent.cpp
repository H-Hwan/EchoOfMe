// Fill out your copyright notice in the Description page of Project Settings.


#include "AmbushStateComponent.h"
#include "Enemy/EchoEnemyBehaviorComponent.h"
#include "Enemy/EchoEnemy.h"

void UAmbushStateComponent::OnStateEnter()
{
	Super::OnStateEnter();


	CurrentTime = MaxStayTime;


	FVector PeekPoint = EnemyBrain->FindPeekPoint();

	EnemyBrain->RequestMoveTo(PeekPoint);

}

void UAmbushStateComponent::OnStateUpdate(float Delta)
{
	Super::OnStateUpdate(Delta);

	CurrentTime -= Delta;

	if (CurrentTime <= 0.0f)
	{
		EnemyBrain->ChangeState(EFSMState::Lost);
	}


	if (MinimumDistance >= EnemyBrain->GetDistanceToPlayer())
	{
		EnemyBrain->ChangeState(EFSMState::Chase);
	}

}

void UAmbushStateComponent::OnStateExit()
{
	Super::OnStateExit();

	CurrentTime = MaxStayTime;

}
