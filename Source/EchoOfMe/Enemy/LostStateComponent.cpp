// Fill out your copyright notice in the Description page of Project Settings.


#include "LostStateComponent.h"
#include "Enemy/EchoEnemyBehaviorComponent.h"
#include "Enemy/EchoEnemy.h"
#include "TimerManager.h"


void ULostStateComponent::OnStateEnter()
{
	Super::OnStateEnter();

	// ⭐️ 핵심 해결: 상태 컴포넌트가 아닌, '몬스터(EchoEnemy)'의 월드를 가져옵니다!
	EchoEnemy->GetWorld()->GetTimerManager().SetTimer(
		ChangeStateHandle,
		this,
		&ULostStateComponent::TimeToTeleport,
		5.0f,
		false
	);
	/// 모든 사운드 오프
	UE_LOG(LogTemp, Log, TEXT("ULostStateComponent"));

}

void ULostStateComponent::OnStateUpdate(float Delta)
{

	if (EnemyBrain->IsPlayerInDetectedSight())
	{
		EnemyBrain->ChangeState(EFSMState::Chase);
	}
}



void ULostStateComponent::OnStateExit()
{
	Super::OnStateExit();

	///사운드 온
	GetWorld()->GetTimerManager().ClearTimer(ChangeStateHandle);

}

void ULostStateComponent::EchosTeleport(FVector Loc)
{
	if (!EchoEnemy || !EnemyBrain) return;
	if (Loc == FVector::ZeroVector)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Lost] 갈 깃발이 없어서 제자리에서 순찰을 시작합니다."));
		EnemyBrain->ChangeState(EFSMState::Patrol);
		return;
	}
	FVector SafeLoc = Loc + FVector(0.0f, 0.0f, 90.0f);
	EchoEnemy->TeleportTo(SafeLoc,EchoEnemy->GetActorRotation());
	EnemyBrain->ChangeState(EFSMState::Patrol);
}

void ULostStateComponent::TimeToTeleport()
{
	// 타이머 5초후 텔레포트
	EchosTeleport(EnemyBrain->PickTeleportToNewPoint());
}

