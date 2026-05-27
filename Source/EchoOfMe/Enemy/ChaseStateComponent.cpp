// Fill out your copyright notice in the Description page of Project Settings.


#include "ChaseStateComponent.h"
#include "EchoOfMe/Enemy/EchoEnemyBehaviorComponent.h"

void UChaseStateComponent::OnStateEnter()
{
	Super::OnStateEnter();

	UE_LOG(LogTemp, Warning, TEXT("[추격]나왜여기임"));

}

void UChaseStateComponent::OnStateUpdate(float Delta)
{
	Super::OnStateUpdate(Delta);



}

void UChaseStateComponent::OnStateExit()
{
	Super::OnStateExit();



}
