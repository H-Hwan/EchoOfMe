// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FlashlightComponent.h"
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
}


void UFlashlightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsVisible()) return;

	const float TargetRadius = CalculateTargetRadius();
	const float NewRadius = FMath::FInterpTo(AttenuationRadius, TargetRadius, DeltaTime, RadiusInterpSpeed);

	SetAttenuationRadius(NewRadius);
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
	const bool bHit = World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams);
	if (!bHit) return MaxAttenuationRadius;

	return FMath::Clamp(Hit.Distance, MinAttenuationRadius, MaxAttenuationRadius);
}


void UFlashlightComponent::SetFlashLightOn(bool bOn) {
	SetVisibility(bOn);
}
