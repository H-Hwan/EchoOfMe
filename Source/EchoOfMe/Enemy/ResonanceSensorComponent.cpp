// Fill out your copyright notice in the Description page of Project Settings.

#include "ResonanceSensorComponent.h"
#include "Enemy/EchoEnemy.h"
#include "Enemy/EchoEnemyBehaviorComponent.h"


#include "EchoGameManager.h"
#include "Component/RecorderComponent.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "Engine/Engine.h"
#include "Engine/World.h"



// Sets default values for this component's properties
UResonanceSensorComponent::UResonanceSensorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UResonanceSensorComponent::LightSensorActivate()
{



}

namespace {
	const TCHAR* LevelText(ESenseLevel L) {
		switch (L) {
		case ESenseLevel::Suspicious: return TEXT("의심");
		case ESenseLevel::Alert:      return TEXT("추적");
		default:                      return TEXT("평온");
		}
	}
	const TCHAR* ChannelText(ESenseChannel C) {
		return C == ESenseChannel::Sound ? TEXT("소리") : TEXT("공명");
	}
}

// Called when the game starts
void UResonanceSensorComponent::BeginPlay()
{
	Super::BeginPlay();

	// 플레이어 캐시
	CachedPlayer = UGameplayStatics::GetPlayerPawn(this, 0);

	EnemyBrain = GetOwner()->FindComponentByClass<UEchoEnemyBehaviorComponent>();
	EchoEnemy = Cast<AEchoEnemy>(GetOwner());

	// 연결: GameManager 방송 구독
	if (UEchoGameManager* GM = UEchoGameManager::Get(this))
	{
		GM->OnMemoryFlagAdded.AddDynamic(this, &UResonanceSensorComponent::HandleMemoryFlagAdded);
		GM->OnRecorderPlaybackCountChanged.AddDynamic(this, &UResonanceSensorComponent::HandleRecorderPlaybackChanged);
		GM->OnNoiseEmitted.AddDynamic(this, &UResonanceSensorComponent::HandleNoiseEmitted);
	}

	// 연결: 녹음기 완청 구독 (플레이어 준비 타이밍 대비 Tick에서 재시도)
	TryBindRecorder();
}


void UResonanceSensorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UEchoGameManager* GM = UEchoGameManager::Get(this))
	{
		GM->OnMemoryFlagAdded.RemoveDynamic(this, &UResonanceSensorComponent::HandleMemoryFlagAdded);
		GM->OnRecorderPlaybackCountChanged.RemoveDynamic(this, &UResonanceSensorComponent::HandleRecorderPlaybackChanged);
		GM->OnNoiseEmitted.RemoveDynamic(this, &UResonanceSensorComponent::HandleNoiseEmitted);
	}

	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			if (URecorderComponent* Rec = PC->FindComponentByClass<URecorderComponent>())
			{
				Rec->OnPlaybackFinished.RemoveDynamic(this, &UResonanceSensorComponent::HandleRecordingFinished);
			}
		}
	}

	Super::EndPlay(EndPlayReason);
}


// Called every frame
void UResonanceSensorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!CachedPlayer)   CachedPlayer = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!bRecorderBound) TryBindRecorder();

	// --- 소리 연속 (달리기 +, 정지 - / 걷기는 변화 없음) ---
	if (CachedPlayer)
	{
		const FVector PlayerLoc = CachedPlayer->GetActorLocation();
		const float Speed = CachedPlayer->GetVelocity().Size();

		if (IsWithinHearingRange(PlayerLoc))
		{
			if (Speed >= RunSpeedThreshold)
			{
				SoundGauge = FMath::Min(SoundGauge + RunGainPerSec * DeltaTime, SoundGaugeMax);
				LastNoiseLocation = PlayerLoc;
			}
			else if (Speed <= IdleSpeedThreshold)
			{
				SoundGauge = FMath::Max(SoundGauge - IdleDecayPerSec * DeltaTime, 0.f);
			}
			// 걷기 구간: 변화 없음 (설계: 걷기 +0)
		}
		else if (Speed <= IdleSpeedThreshold)
		{
			// 사거리 밖에서 멈춰 있어도 천천히 가라앉음
			SoundGauge = FMath::Max(SoundGauge - IdleDecayPerSec * DeltaTime, 0.f);
		}
	}
	UpdateChannelLevel(ESenseChannel::Sound, SoundGauge, SoundLevel);

	// --- 공명 연속 (플레이어 공명값을 그대로 감지) ---
	UpdateChannelLevel(ESenseChannel::Resonance, GetResonance(), ResonanceLevel);

	// --- 디버그 표시 ---
	if (bDrawDebug && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(7001, 0.f, FColor::Cyan,
			FString::Printf(TEXT("[Sense] 소리 %.0f(%s) | 공명 %.0f(%s) | 위협 %s"),
				SoundGauge, LevelText(SoundLevel),
				GetResonance(), LevelText(ResonanceLevel),
				LevelText(GetThreatLevel())));
	}
}


void UResonanceSensorComponent::LightSensorActivate()
{



}


void UResonanceSensorComponent::SoundSensorActivate(ESoundCue Cue, FVector NoiseLocation)
{
	if (!IsWithinHearingRange(NoiseLocation)) return;

	float Amount = 0.f;
	switch (Cue)
	{
	case ESoundCue::DoorRough:      Amount = DoorRoughAmount;      break;
	case ESoundCue::ItemDrop:       Amount = ItemDropAmount;       break;
	case ESoundCue::HideFail:       Amount = HideFailAmount;       break;
	case ESoundCue::RecorderManual: Amount = RecorderManualAmount; break;
	}

	AddSoundNoise(Amount, NoiseLocation);
}


void UResonanceSensorComponent::SuperNaturalPhenomenonSensor(EResonanceCue Cue)
{
	UEchoGameManager* GM = UEchoGameManager::Get(this);
	if (!GM) return;

	float Amount = 0.f;
	switch (Cue)
	{
	case EResonanceCue::MemoryFragment: Amount = MemoryFragmentAmount; break;
	case EResonanceCue::RecordingFull:  Amount = RecordingFullAmount;  break;
	}

	// 공명은 단일 값(GameManager)으로 관리 — 플레이어 공명과 같은 축에 누적
	const float New = FMath::Clamp(GM->GetResonance() + Amount, 0.f, MaxResonanceCount);
	GM->SetResonance(New);

	UpdateChannelLevel(ESenseChannel::Resonance, New, ResonanceLevel);
}


void UResonanceSensorComponent::ResetSoundSense()
{
	SoundGauge = 0.f;
	LastNoiseLocation = FVector::ZeroVector;
	UpdateChannelLevel(ESenseChannel::Sound, SoundGauge, SoundLevel);
}


//---
// 자동 연결 핸들러

void UResonanceSensorComponent::HandleMemoryFlagAdded(FName /*FlagKey*/)
{
	// 기억 단서 획득 → 공명 +25
	SuperNaturalPhenomenonSensor(EResonanceCue::MemoryFragment);
}

void UResonanceSensorComponent::HandleRecorderPlaybackChanged(int32 /*NewCount*/)
{
	// 녹음기 수동 재생 → 소리 +30 (플레이어 위치)
	const FVector Loc = CachedPlayer ? CachedPlayer->GetActorLocation() : GetSensorLocation();
	SoundSensorActivate(ESoundCue::RecorderManual, Loc);
}

void UResonanceSensorComponent::HandleRecordingFinished()
{
	// 기억 녹음 완청 → 공명 +40
	// [주의] AudioComponent는 수동 Stop 시에도 Finished를 쏠 수 있음.
	//        "끝까지 들음"만 잡고 싶으면 녹음기 쪽에서 완료/중단 구분 플래그 추가 권장.
	SuperNaturalPhenomenonSensor(EResonanceCue::RecordingFull);
}

void UResonanceSensorComponent::HandleNoiseEmitted(float Amount, FVector Location)
{
	// 문/물건/숨기 등 환경 소음 → 소리 게이지에 직접 가중
	AddSoundNoise(Amount, Location);
}


//---
// 내부

void UResonanceSensorComponent::TryBindRecorder()
{
	UWorld* World = GetWorld();
	if (!World) return;

	APlayerController* PC = World->GetFirstPlayerController();
	URecorderComponent* Rec = PC ? PC->FindComponentByClass<URecorderComponent>() : nullptr;
	if (!Rec) return;

	Rec->OnPlaybackFinished.AddDynamic(this, &UResonanceSensorComponent::HandleRecordingFinished);
	bRecorderBound = true;
}


void UResonanceSensorComponent::AddSoundNoise(float Amount, const FVector& Location)
{
	if (!IsWithinHearingRange(Location)) return;

	SoundGauge = FMath::Clamp(SoundGauge + Amount, 0.f, SoundGaugeMax);
	LastNoiseLocation = Location;

	UpdateChannelLevel(ESenseChannel::Sound, SoundGauge, SoundLevel);
}


float UResonanceSensorComponent::GetResonance() const
{
	UEchoGameManager* GM = UEchoGameManager::Get(this);
	return GM ? GM->GetResonance() : 0.f;
}


ESenseLevel UResonanceSensorComponent::GetThreatLevel() const
{
	return (uint8)SoundLevel >= (uint8)ResonanceLevel ? SoundLevel : ResonanceLevel;
}


ESenseLevel UResonanceSensorComponent::EvaluateLevel(float Value) const
{
	if (Value >= ChaseThreshold)   return ESenseLevel::Alert;
	if (Value >= SuspectThreshold) return ESenseLevel::Suspicious;
	return ESenseLevel::Calm;
}


void UResonanceSensorComponent::UpdateChannelLevel(ESenseChannel Channel, float Value, ESenseLevel& InOutLevel)
{
	const ESenseLevel NewLevel = EvaluateLevel(Value);
	if (NewLevel != InOutLevel)
	{
		InOutLevel = NewLevel;
		OnSenseLevelChanged.Broadcast(Channel, NewLevel);
	}
}


FVector UResonanceSensorComponent::GetSensorLocation() const
{
	const AActor* Owner = GetOwner();
	if (!Owner) return FVector::ZeroVector;

	// 컨트롤러에 붙은 경우 폰 위치 사용
	if (const AController* AsController = Cast<AController>(Owner))
	{
		if (const APawn* Pawn = AsController->GetPawn())
		{
			return Pawn->GetActorLocation();
		}
	}
	return Owner->GetActorLocation();
}


bool UResonanceSensorComponent::IsWithinHearingRange(const FVector& Location) const
{
	if (HearingRange <= 0.f) return true;
	return FVector::DistSquared(GetSensorLocation(), Location) <= FMath::Square(HearingRange);
}


void UResonanceSensorComponent::NaturalPhenomenonSensor(FVector FlashLightHitLocation)
{
	if (!EchoEnemy || !EnemyBrain->GetPlayerInfo()) return;

	FVector EnemyLocation = EchoEnemy->GetActorLocation();

	FVector PlayerLocation = EnemyBrain->GetPlayerLocation();

	float Distance = FVector::Distance(EnemyLocation, PlayerLocation);

	if (Distance > MaxDistanceToReflectedLight) return;

	FVector ForwardV = EchoEnemy->GetActorForwardVector();

	FVector TDV = (PlayerLocation - EnemyLocation).GetSafeNormal();

	float DotResult = FVector::DotProduct(ForwardV, TDV);

	CosAngle = FMath::Cos(FMath::DegreesToRadians(MaxLightDetectSight));

	if (DotResult <= CosAngle) return;

	FHitResult HitResult;
	
	FCollisionQueryParams Param;

	Param.AddIgnoredActor(EchoEnemy);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, EnemyLocation + FVector(0.0f, 0.0f, 60.0f), PlayerLocation + FVector(0.0f, 0.0f, 60.0f), ECC_Pawn, Param);

	if (bHit && HitResult.GetActor() == EnemyBrain->GetPlayerInfo())
	{
		DrawDebugLine(GetWorld(), EnemyLocation + FVector(0.0f, 0.0f, 60.0f), PlayerLocation + FVector(0.0f, 0.0f, 60.0f), FColor::Purple, false, -1.0f, 0, 2.0f);
		return;
	}

	return;
	
}

