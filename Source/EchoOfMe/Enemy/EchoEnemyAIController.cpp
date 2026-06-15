// Fill out your copyright notice in the Description page of Project Settings.


#include "EchoEnemyAIController.h"
#include "Enemy/EchoEnemy.h"

#include "Kismet/GameplayStatics.h"


void AEchoEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	//OwnerPawn = Cast<AEchoEnemy>(InPawn);

	//// 센서 탐색: 컨트롤러에 붙었으면 this, 폰에 붙었으면 InPawn에서
	//CachedSensor = FindComponentByClass<UResonanceSensorComponent>();
	//if (!CachedSensor && InPawn)
	//{
	//	CachedSensor = InPawn->FindComponentByClass<UResonanceSensorComponent>();
	//}

	//if (CachedSensor)
	//{
	//	CachedSensor->OnSenseLevelChanged.AddDynamic(this, &AEchoEnemyAIController::HandleSenseLevelChanged);
	//	UE_LOG(LogTemp, Warning, TEXT("[EnemyAI] 센서 연결 완료"));
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Error, TEXT("[EnemyAI] ResonanceSensorComponent 못 찾음 - 폰/컨트롤러 어디에 붙었는지 확인"));
	//}
}


void AEchoEnemyAIController::OnUnPossess()
{
	//if (CachedSensor)
	//{
	//	CachedSensor->OnSenseLevelChanged.RemoveDynamic(this, &AEchoEnemyAIController::HandleSenseLevelChanged);
	//}

	//Super::OnUnPossess();
}


void AEchoEnemyAIController::HandleSenseLevelChanged(ESenseChannel Channel, ESenseLevel Level)
{
	//if (!CachedSensor) return;

	//if (Level == ESenseLevel::Alert)   // 추적
	//{
	//	FVector Target;
	//	if (Channel == ESenseChannel::Sound)
	//	{
	//		Target = CachedSensor->GetLastNoiseLocation();
	//		UE_LOG(LogTemp, Warning, TEXT("[EnemyAI] 소리 추적 -> %s"), *Target.ToString());
	//	}
	//	else
	//	{
	//		APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0);
	//		if (!Player) return;
	//		Target = Player->GetActorLocation();
	//		UE_LOG(LogTemp, Warning, TEXT("[EnemyAI] 공명 추적 -> %s"), *Target.ToString());
	//	}
	//	MoveToLocation(Target, 100.f);
	//}
	//else if (Level == ESenseLevel::Calm)
	//{
	//	StopMovement();
	//}
}
