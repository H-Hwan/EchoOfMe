// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/FlashlightComponent.h"
#include "Engine/World.h"


UFlashlightComponent::UFlashlightComponent() {
	PrimaryComponentTick.bCanEverTick = true;

	SetOuterConeAngle(20.f);
	SetInnerConeAngle(0.f);
	SetAttenuationRadius(MaxAttenuationRadius);
}


void UFlashlightComponent::BeginPlay() {
	Super::BeginPlay();

	SetAttenuationRadius(MaxAttenuationRadius);

	bIsOn = false;
	SetVisibility(false);
}


void UFlashlightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsVisible()) return;

	const float TargetRadius = CalculateTargetRadius();
	const float NewRadius = FMath::FInterpTo(AttenuationRadius, TargetRadius, DeltaTime, RadiusInterpSpeed);

	SetAttenuationRadius(NewRadius);

	// 의심 게이지: ON일 때 누적, OFF일 때 감쇠
	if (bIsOn) {
		Suspicion = FMath::Min(Suspicion + SuspicionGainPerSec * DeltaTime, SuspicionThreshold);

		if (Suspicion >= SuspicionThreshold && !bIsLocked) {
			TriggerLightFailure();
		}
	}
	else if (Suspicion > 0.f) {
		Suspicion = FMath::Max(Suspicion - SuspicionDecayPerSec * DeltaTime, 0.f);
	}

	UE_LOG(LogTemp, Verbose, TEXT("[Flashlight] Suspicion=%.1f"), Suspicion);
}


float UFlashlightComponent::CalculateTargetRadius() const {
	const UWorld* World = GetWorld();
	if (!World) return MaxAttenuationRadius;

	const FVector Start = GetComponentLocation();
	const FVector End = Start + GetForwardVector() * MaxAttenuationRadius;

	FCollisionQueryParams QueryParams;
	// 플레이어 무시
	QueryParams.AddIgnoredActor(GetOwner());

	FHitResult Hit;
	const bool bHit = World->LineTraceSingleByChannel(Hit, Start, End, ECC_Camera, QueryParams);
	if (!bHit) return MaxAttenuationRadius;

	return FMath::Clamp(Hit.Distance, MinAttenuationRadius, MaxAttenuationRadius);
}


void UFlashlightComponent::ToggleFlashLight() {
	if (bIsLocked) return; // 빛의 실패 중엔 입력 무시
	SetFlashLightOn(!bIsOn);
}


void UFlashlightComponent::SetFlashLightOn(bool bOn) {
	bIsOn = bOn;
	SetVisibility(bOn);
}


void UFlashlightComponent::TriggerLightFailure() {
	if (bIsLocked) return;

	bIsLocked = true;
	Suspicion = 0.f;   // 발동 후 게이지 리셋
	SetFlashLightOn(false);

	UE_LOG(LogTemp, Log, TEXT("[Flashlight] 빛의 실패 발동"));

	if (UWorld* World = GetWorld()) {
		World->GetTimerManager().SetTimer(ForcedOffTimerHandle,
			this, &UFlashlightComponent::EndLightFailure, ForcedOffDuration, false);
	}
}

FVector UFlashlightComponent::LightTrace()
{
	FHitResult HitResult;

	FVector StartLocation = GetOwner()->GetActorLocation();
	FVector EndLocation = StartLocation + (GetOwner()->GetActorForwardVector() * 1000.0f);

	FCollisionQueryParams TraceParams(FName(TEXT("MyTrace")), true, GetOwner());

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Camera, TraceParams);

	if (bHit)
	{
		return HitResult.ImpactPoint;
	}

	return EndLocation;

}


void UFlashlightComponent::EndLightFailure() {
	bIsLocked = false;

	// TODO(사운드): 빛의 실패 효과음 — 지직거림, 전구 깜빡임 직전 소리

	UE_LOG(LogTemp, Log, TEXT("[Flashlight] 빛의 실패 해제"));
}
