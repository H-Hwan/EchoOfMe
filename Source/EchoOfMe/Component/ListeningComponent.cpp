// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/ListeningComponent.h"

#include "EchoGameManager.h"

#include "TimerManager.h"
#include "Engine/World.h"


// Sets default values for this component's properties
UListeningComponent::UListeningComponent() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UListeningComponent::BeginPlay() {
	Super::BeginPlay();

	bIsListening = false;
}


void UListeningComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// GameManager에서 현재 값 읽기
	UEchoGameManager* GM = UEchoGameManager::Get(this);
	if (!GM) return;

	float CurrentResonance = GM->GetResonance();
	float NewResonance = CurrentResonance;

	if (bIsListening) {
		NewResonance = FMath::Min(CurrentResonance + ResonanceGainPerSec * DeltaTime, ResonanceMax);
	}
	else if (CurrentResonance > 0.f) {
		NewResonance = FMath::Max(CurrentResonance - ResonanceDecayPerSec * DeltaTime, 0.f);
	}

	if (!FMath::IsNearlyEqual(NewResonance, CurrentResonance)) {
		GM->SetResonance(NewResonance);
	}
}


// 듣기 시작
void UListeningComponent::StartListening() {
	// 최소 시간 종료 대개중이라면, 새 입력으로 취소 >> 듣기 유지
	if (UWorld* World = GetWorld()) {
		World->GetTimerManager().ClearTimer(PendingStopHandle);
	}

	if (bIsListening) return;

	bIsListening = true;
	ListenStartedAt = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;

	UE_LOG(LogTemp, Log, TEXT("[Listening] 듣기 시작"));

	/*	[ToDo] 환경금 증폭 시작
		- 저역 강조 EQ
		- 공명 앰비언스 페이드 인	*/

	OnListeningChanged.Broadcast(true);
}


// 듣기 종료
void UListeningComponent::StopListening() {
	if (!bIsListening) return;

	UWorld* World = GetWorld();
	if (!World) {
		PerformStop();
		return;
	}

	// 최소 홀드 시간 보장
	const float Elapsed = World->GetTimeSeconds() - ListenStartedAt;
	const float Remaining = MinHoldDuration - Elapsed;

	if (Remaining > 0.f) {
		World->GetTimerManager().SetTimer(PendingStopHandle, this, &UListeningComponent::PerformStop, Remaining, false);
	}
	else PerformStop();
}


void UListeningComponent::PerformStop() {
	if (!bIsListening) return;

	bIsListening = false;

	const float CurResonance = UEchoGameManager::Get(this) ? UEchoGameManager::Get(this)->GetResonance() : 0.f;
	UE_LOG(LogTemp, Log, TEXT("[Listening] 종료 (공명 %.1f)"), CurResonance);

	/*	[ToDo] 듣기 종료 처리
		- 환경음 필터링 복원
		- 공명 앰비언스 페이드 아웃	*/

	// 큐 같이 비움
	SetCurrentCue(EListeningCue::None);

	OnListeningChanged.Broadcast(false);
}


void UListeningComponent::SetCurrentCue(EListeningCue NewCue) {
	if (CurrentCue == NewCue) return;

	CurrentCue = NewCue;
	UE_LOG(LogTemp, Log, TEXT("[Listening] 큐 변경: %d)"), (int32)NewCue);

	OnCueChanged.Broadcast(NewCue);
}
