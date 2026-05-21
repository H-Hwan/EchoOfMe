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
}


// 인벤토리에서 아이템 사용되면 호출
void URecorderComponent::HandleItemUsed(UInventoryItemDefinition* Item) {

}


// 현재 단계의 음성 재생
void URecorderComponent::PlayCurrentStage() {

}


// 재생 중인 음성 중단
void URecorderComponent::StopPlayBack() {

}


// 인식 단계 한 칸 진행
void URecorderComponent::AdvanceStage() {

}


// 재생 중 여부 반환
bool URecorderComponent::IsPlaying() const {

}


// 음성 재생이 끝났을 때 콜백
void URecorderComponent::HandleAudioFinished() {

}
