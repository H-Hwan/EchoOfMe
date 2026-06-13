// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/EnemyCueComponent.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"


void UEnemyCueComponent::ResolveCueData()
{
	CueType = EListeningCue::Echo;   // 잔향 큐 (AmbientSound/ListeningSound는 에디터에서 설정, 선택)
}


void UEnemyCueComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!Owner) return;

	// 센서 탐색: 오너 우선, 없으면 폰/컨트롤러 쪽
	Sensor = Owner->FindComponentByClass<UResonanceSensorComponent>();
	if (!Sensor)
	{
		if (APawn* P = Cast<APawn>(Owner))
		{
			if (AController* C = P->GetController())
				Sensor = C->FindComponentByClass<UResonanceSensorComponent>();
		}
		else if (AController* C = Cast<AController>(Owner))
		{
			if (APawn* ControlledPawn = C->GetPawn())
				Sensor = ControlledPawn->FindComponentByClass<UResonanceSensorComponent>();
		}
	}

	if (Sensor)
	{
		LastThreat = Sensor->GetThreatLevel();
		Sensor->OnSenseLevelChanged.AddDynamic(this, &UEnemyCueComponent::HandleSenseLevelChanged);
	}
}


void UEnemyCueComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Sensor)
	{
		Sensor->OnSenseLevelChanged.RemoveDynamic(this, &UEnemyCueComponent::HandleSenseLevelChanged);
	}

	Super::EndPlay(EndPlayReason);
}


void UEnemyCueComponent::HandleSenseLevelChanged(ESenseChannel /*Channel*/, ESenseLevel /*Level*/)
{
	if (!Sensor) return;

	// 채널별이 아니라 종합 위협 단계가 바뀔 때만 스팅어
	const ESenseLevel Now = Sensor->GetThreatLevel();
	if (Now == LastThreat) return;
	LastThreat = Now;

	switch (Now)
	{
	case ESenseLevel::Suspicious: PlayStinger(StingerSuspicious); break;
	case ESenseLevel::Alert:      PlayStinger(StingerAlert);      break;
	default:                      PlayStinger(StingerCalm);       break;
	}
}


void UEnemyCueComponent::PlayStinger(USoundBase* Sound) const
{
	const AActor* Owner = GetOwner();
	if (!Sound || !Owner) return;

	UGameplayStatics::SpawnSoundAtLocation(this, Sound, Owner->GetActorLocation());
}
