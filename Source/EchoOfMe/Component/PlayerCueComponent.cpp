// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/PlayerCueComponent.h"

#include "EchoGameManager.h"

#include "Components/AudioComponent.h"
#include "Curves/CurveFloat.h"


UPlayerCueComponent::UPlayerCueComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UPlayerCueComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!Owner || !ResonanceLoopSound) return;

	ResonanceAudio = NewObject<UAudioComponent>(Owner);
	if (!ResonanceAudio) return;

	ResonanceAudio->bAutoActivate = true;
	ResonanceAudio->bAllowSpatialization = false;   // 2D
	ResonanceAudio->SetSound(ResonanceLoopSound);
	ResonanceAudio->SetupAttachment(Owner->GetRootComponent());
	ResonanceAudio->RegisterComponent();
	ResonanceAudio->SetVolumeMultiplier(0.f);        // 공명 0에서 무음 시작
}


void UPlayerCueComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!ResonanceAudio) return;

	UEchoGameManager* GM = UEchoGameManager::Get(this);
	const float R = GM ? GM->GetResonance() : 0.f;

	float Alpha = FMath::Clamp(R / FMath::Max(ResonanceMax, 1.f), 0.f, 1.f);
	if (ResponseCurve) Alpha = ResponseCurve->GetFloatValue(Alpha);

	ResonanceAudio->SetVolumeMultiplier(Alpha * MaxVolume);
}
