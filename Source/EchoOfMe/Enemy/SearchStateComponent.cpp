// Fill out your copyright notice in the Description page of Project Settings.


#include "SearchStateComponent.h"
#include "EchoOfMe/Enemy/EchoEnemyBehaviorComponent.h"
#include"Enemy/EchoEnemy.h"
void USearchStateComponent::OnStateEnter()
{
	Super::OnStateEnter();

	LastLocation = EnemyBrain->GetPlayerLocation();
	EnemyBrain->RequestMoveTo(LastLocation);

	UE_LOG(LogTemp, Warning, TEXT("[수색 시작] 마지막 추격 지점으로 이동"));

	CurrentRandomChangeSearchToAmbush = FMath::RandRange(0, 10);

}

void USearchStateComponent::OnStateUpdate(float Delta)
{
	Super::OnStateUpdate(Delta);
	// 몬스터의 현재 이동 속도를 구합니다.
	float CurrentSpeed = EchoEnemy->GetVelocity().Size();


	if (EnemyBrain->IsLightDetected())
	{
		UE_LOG(LogTemp, Error, TEXT("[[Search]빛을 봤잖아]"));
		EnemyBrain->RequestMoveTo(EnemyBrain->IsLightLoc());
	}

	// 1. 길을 찾는 중(IsNavMoving)인데 속도가 거의 0(멈춤)이라면?
	if (EnemyBrain->IsNavMoving() && CurrentSpeed < 10.0f)
	{
		StuckTimer += Delta; // 타이머 증가
		if (StuckTimer > 2.0f) // 2초 넘게 끼어있으면!
		{
			UE_LOG(LogTemp, Warning, TEXT("어딘가에 끼었다! 새로운 곳으로 목적지 변경!"));
			EnemyBrain->PickCustomRadiusNavLocation(LastLocation, 600.0f);
			EnemyBrain->RequestMoveTo(LastLocation);
			StuckTimer = 0.0f; // 타이머 초기화
		}
	}
	else
	{
		StuckTimer = 0.0f; // 잘 걷고 있으면 타이머 초기화
	}

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

	UE_LOG(LogTemp, Log, TEXT("[수색지역 이탈] 텔레포팅"));

	WanderHoldTimer = 5.0f;
	StuckTimer = 0.0f;
	ChangeStateTimer = 10.0f;
}
