// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolStateComponent.h"
#include "EchoOfMe/Enemy/EchoEnemyBehaviorComponent.h"
#include "EchoEnemyAIController.h"
#include "Enemy/EchoEnemy.h"
#include "NavigationSystem.h"


void UPatrolStateComponent::OnStateEnter()
{
	Super::OnStateEnter();


	


}

void UPatrolStateComponent::OnStateUpdate(float Delta)
{
	Super::OnStateUpdate(Delta);




}

void UPatrolStateComponent::OnStateExit()
{
	Super::OnStateExit();



}

bool UPatrolStateComponent::PickRandomNavMovePoint(FVector& OutLocation) const
{
	if (!EchoEnemy) return false;

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(EchoEnemy->GetWorld());
	if (!NavSystem) return false;

	FNavLocation NavLocation;

	const bool bNavFount = NavSystem->GetRandomReachablePointInRadius(EchoEnemy->GetActorLocation, EchoEnemy->PatrolRadius, NavLocation);

	if (bNavFount) {
		OutLocation = NavLocation;
	}

	return false;
}
