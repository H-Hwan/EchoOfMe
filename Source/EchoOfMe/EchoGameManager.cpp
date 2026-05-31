// Fill out your copyright notice in the Description page of Project Settings.


#include "EchoGameManager.h"

#include "Engine/World.h"


void UEchoGameManager::AddMemoryFlag(FName FlagKey) {
	if (FlagKey.IsNone()) return;

	/*	[TSet.Add(T, &bool)] 
		이미 Set 안에 있는 요소라면 매개변수로 넣은 bool변수를 true,
		새로 들어온 요소라면 false 반환	*/ 
	bool bAlreadyHad = false;
	CollectedMemoryFlags.Add(FlagKey, &bAlreadyHad);
	if (bAlreadyHad) return;

	UE_LOG(LogTemp, Log, TEXT("[EchoGameManager] 메모리 플래그 추가: %s (누적: %d)"), *FlagKey.ToString(), CollectedMemoryFlags.Num());

	OnMemoryFlagAdded.Broadcast(FlagKey);
}


void UEchoGameManager::IncrementRecorderPlayBack() {
	RecorderPlaybackCount++;

	UE_LOG(LogTemp, Log, TEXT("[EchoGameManager] 녹음기 재생 횟수: %d"), RecorderPlaybackCount);

	OnRecorderPlaybackCountChanged.Broadcast(RecorderPlaybackCount);
}


void UEchoGameManager::SetResonance(float NewResonance) {
	if (FMath::IsNearlyEqual(Resonance, NewResonance)) return;

	Resonance = NewResonance;
	UE_LOG(LogTemp, Verbose, TEXT("[EchoGameManager] 공명: %.1f"), Resonance);

	OnResonanceChanged.Broadcast(Resonance);
}


UEchoGameManager* UEchoGameManager::Get(const UObject* WorldContextObject) {
	if (!WorldContextObject) return nullptr;

	if (UWorld* World = WorldContextObject->GetWorld()) {
		return World->GetSubsystem<UEchoGameManager>();
	}

	return nullptr;
}
