// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/StoryPlayerComponent.h"

#include "Component/MemoryComponent.h"
#include "Data/MemoryFragmentDefinition.h"
#include "Data/StorySequence.h"
#include "Item/EndingDoorActor.h"
#include "StoryWidget.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UStoryPlayerComponent::UStoryPlayerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UStoryPlayerComponent::BeginPlay()
{
	Super::BeginPlay();

	// 회수음 종료 → 스토리 컷
	if (UMemoryComponent* Memory = GetOwner()->FindComponentByClass<UMemoryComponent>()) {
		Memory->OnFlashbackFinished.AddDynamic(this, &UStoryPlayerComponent::HandleFlashbackFinished);
		BoundMemory = Memory;
	}

	// 엔딩 문 → 엔딩 컷
	BindEndingDoors();
}


void UStoryPlayerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (BoundMemory) {
		BoundMemory->OnFlashbackFinished.RemoveDynamic(this, &UStoryPlayerComponent::HandleFlashbackFinished);
		BoundMemory = nullptr;
	}

	for (AEndingDoorActor* Door : BoundDoors) {
		if (Door) {
			Door->OnEndingDoorOpened.RemoveDynamic(this, &UStoryPlayerComponent::HandleEndingDoorOpened);
		}
	}
	BoundDoors.Reset();

	Super::EndPlay(EndPlayReason);
}


void UStoryPlayerComponent::BindEndingDoors() {
	TArray<AActor*> Doors;
	UGameplayStatics::GetAllActorsOfClass(this, AEndingDoorActor::StaticClass(), Doors);

	for (AActor* Actor : Doors) {
		if (AEndingDoorActor* Door = Cast<AEndingDoorActor>(Actor)) {
			Door->OnEndingDoorOpened.AddDynamic(this, &UStoryPlayerComponent::HandleEndingDoorOpened);
			BoundDoors.Add(Door);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[StoryPlayer] 엔딩 문 %d개 구독"), BoundDoors.Num());
}


void UStoryPlayerComponent::HandleFlashbackFinished(const UMemoryFragmentDefinition* Definition)
{
	// 컷이 없는 조각이면 무시
	if (!Definition || !Definition->Story) return;
	if (!StoryWidgetClass) return;

	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC || !PC->IsLocalController()) return;

	// 이전 컷이 아직 화면에 남아 있으면 정리
	if (ActiveStoryWidget && ActiveStoryWidget->IsInViewport()) {
		ActiveStoryWidget->RemoveFromParent();
	}

	ActiveStoryWidget = CreateWidget<UStoryWidget>(PC, StoryWidgetClass);
	if (ActiveStoryWidget) {
		// AddToViewport / Pause / 페이지 진행은 위젯 BP가 자체 처리
		ActiveStoryWidget->PlaySequence(Definition->Story);
	}
}


void UStoryPlayerComponent::HandleEndingDoorOpened(EEndingType Ending, UStorySequence* EndingStory) {
	// 엔딩 종류와 무관하게 연결된 컷 재생 (차이는 EndingStory 데이터에 있음)
	PlayStory(EndingStory);
}


void UStoryPlayerComponent::PlayStory(UStorySequence* Sequence) {
	if (!Sequence || !StoryWidgetClass) return;

	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC || !PC->IsLocalController()) return;

	// 이전 컷이 아직 화면에 남아 있으면 정리
	if (ActiveStoryWidget && ActiveStoryWidget->IsInViewport()) {
		ActiveStoryWidget->RemoveFromParent();
	}

	ActiveStoryWidget = CreateWidget<UStoryWidget>(PC, StoryWidgetClass);
	if (ActiveStoryWidget) {
		// AddToViewport / Pause / 페이지 진행 / NextLevelName 라우팅은 위젯 BP가 처리
		ActiveStoryWidget->PlaySequence(Sequence);
	}
}
