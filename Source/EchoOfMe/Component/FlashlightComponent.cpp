#include "Component/FlashlightComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h" // 사운드 재생 함수를 사용하기 위해 헤더 추가

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
	LightTrace();
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
	// 기존 상태와 다를 때만 사운드가 1번 나도록 방어 조건 추가

	if (bIsOn != bOn)
	{
		if (bOn)
		{
			if (SoundFlashlightOn)
			{
				UGameplayStatics::PlaySound2D(this, SoundFlashlightOn);
			}
		}
		else
		{
			// 빛의 실패 상태(bIsLocked)일 때는 일반 꺼짐 소리가 중복해서 나지 않도록 차단
			if (SoundFlashlightOff && !bIsLocked)
			{
				UGameplayStatics::PlaySound2D(this, SoundFlashlightOff);
			}
		}
	}

	bIsOn = bOn;
	SetVisibility(bOn);
}

void UFlashlightComponent::TriggerLightFailure() {
	if (bIsLocked) return;

	bIsLocked = true;
	Suspicion = 0.f;   // 발동 후 게이지 리셋

	// 빛의 실패 전용 효과음 재생 (지직거리는 소리)
	if (SoundLightFailure)
	{
		UGameplayStatics::PlaySound2D(this, SoundLightFailure);
	}

	SetFlashLightOn(false);

	UE_LOG(LogTemp, Log, TEXT("[Flashlight] 빛의 실패 발동"));

	if (UWorld* World = GetWorld()) {
		World->GetTimerManager().SetTimer(ForcedOffTimerHandle,
			this, &UFlashlightComponent::EndLightFailure, ForcedOffDuration, false);
	}
}

void UFlashlightComponent::LightTrace()
{
	FHitResult HitResult;

	FVector StartLocation = GetComponentLocation();
	FVector EndLocation = StartLocation + GetForwardVector() * MaxAttenuationRadius;

	FCollisionQueryParams TraceParams(FName(TEXT("FlashLightTrace")), true, GetOwner());

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		ECC_WorldStatic,
		TraceParams);

	CachedLightHitPoint =
		bHit ? HitResult.ImpactPoint : EndLocation;
}

FVector UFlashlightComponent::LightEndPoint()
{
	return CachedLightHitPoint;
}

bool UFlashlightComponent::IsLightTurnOn()
{
	return bIsOn;
}

void UFlashlightComponent::EndLightFailure() {
	bIsLocked = false;
	UE_LOG(LogTemp, Log, TEXT("[Flashlight] 빛의 실패 해제"));
}
