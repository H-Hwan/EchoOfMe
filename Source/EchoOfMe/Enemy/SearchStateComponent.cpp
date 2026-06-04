// Fill out your copyright notice in the Description page of Project Settings.


#include "SearchStateComponent.h"
#include "EchoOfMe/Enemy/EchoEnemyBehaviorComponent.h"
#include"Enemy/EchoEnemy.h"
void USearchStateComponent::OnStateEnter()
{
	Super::OnStateEnter();

	LastLocation = EnemyBrain->GetPlayerLocation();
	EnemyBrain->RequestMoveTo(LastLocation);

	UE_LOG(LogTemp, Error, TEXT("[수색 시작] 마지막 추격 지점으로 이동"));

	CurrentRandomChangeSearchToAmbush = FMath::RandRange(0, 10);

}

void USearchStateComponent::OnStateUpdate(float Delta)
{
	Super::OnStateUpdate(Delta);


	if (EnemyBrain->IsPlayerInDetectedSight())
	{
		if (SearchToAmbush >= CurrentRandomChangeSearchToAmbush)// 지금 스토킹 포인트 이동
		{
			UE_LOG(LogTemp, Error, TEXT("[스토킹 시작]"));
			EnemyBrain->ChangeState(EFSMState::Ambush);
			return;
		}
		UE_LOG(LogTemp, Error, TEXT("[수색중 적발견]"));
		EnemyBrain->ChangeState(EFSMState::Chase);
		return;
	}
	
	if (FVector::Dist2D(LastLocation, EchoEnemy->GetActorLocation()) > 100.0f)
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

	UE_LOG(LogTemp, Error, TEXT("[수색지역 이탈] 텔레포팅"));

	WanderHoldTimer = 5.0f;

	ChangeStateTimer = 10.0f;
}
