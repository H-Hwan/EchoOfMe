// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/RecorderComponent.h"

#include "Component/InventoryComponent.h"
#include "Data/RecorderItemDefinition.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"



// Sets default values for this component's properties
URecorderComponent::URecorderComponent() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void URecorderComponent::BeginPlay() {
	Super::BeginPlay();

	// 인벤토리 아이템 사용 메소드 바인딩
	if (UInventoryComponent* Inventory = GetOwner()->FindComponentByClass<UInventoryComponent>()) {
		Inventory->OnItemUsed.AddDynamic(this, &URecorderComponent::HandleItemUsed);
	}
}


// 인벤토리에서 아이템 사용되면 호출
void URecorderComponent::HandleItemUsed(UInventoryItemDefinition* Item) {
	// 사용된 게 녹음기일 때만 사용
	if (Item && Item == RecorderDefinition) PlayCurrentStage();
}


// 현재 단계의 음성 재생
void URecorderComponent::PlayCurrentStage() {
	if (!RecorderDefinition || RecorderDefinition->StageSounds.Num() == 0) return;
	// 이미 재생 중이라면 무시
	if (IsPlaying()) return;

	const int32 Index = FMath::Clamp(PlaybackStage, 0, RecorderDefinition->StageSounds.Num() - 1);
	USoundBase* Sound = RecorderDefinition->StageSounds[Index];
	if (!Sound) return;

	ActiveAudio = UGameplayStatics::SpawnSound2D(this, Sound);
	if (ActiveAudio) {
		ActiveAudio->OnAudioFinished.AddDynamic(this, &URecorderComponent::HandleAudioFinished);
	}

	OnStagePlayed.Broadcast(Index);
}


// 재생 중인 음성 중단
void URecorderComponent::StopPlayBack() {
	if (ActiveAudio) ActiveAudio->Stop();
}


// 인식 단계 한 칸 진행
void URecorderComponent::AdvanceStage() {
	if (!RecorderDefinition) return;

	// 마지막 단계에서 멈춤
	PlaybackStage = FMath::Min(PlaybackStage + 1, RecorderDefinition->StageSounds.Num() - 1);
}


// 재생 중 여부 반환
bool URecorderComponent::IsPlaying() const {
	return ActiveAudio && ActiveAudio->IsPlaying();
}


// 음성 재생이 끝났을 때 콜백
void URecorderComponent::HandleAudioFinished() {
	if (ActiveAudio) {
		ActiveAudio->OnAudioFinished.RemoveAll(this);
		ActiveAudio = nullptr;
	}

	OnPlaybackFinished.Broadcast();
}
