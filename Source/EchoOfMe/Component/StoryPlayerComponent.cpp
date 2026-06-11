// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/StoryPlayerComponent.h"

#include "Component/MemoryComponent.h"
#include "Data/MemoryFragmentDefinition.h"
#include "Data/StorySequence.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"


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

	// 오너의 MemoryComponent에 회수음 종료 신호 바인딩
	if(UMemoryComponent* Memory = GetOwner()->FindComponentByClass<UMemoryComponent>()){
		Memory->OnFlashbackFinished.AddDynamic(this, &UStoryPlayerComponent::HandleFlashbackFinished);
		BoundMemory = Memory;
	}
}


void UStoryPlayerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(BoundMemory) {
		BoundMemory->OnFlashbackFinished.RemoveDynamic(this, &UStoryPlayerComponent::HandleFlashbackFinished);
		BoundMemory = nullptr;
	}

	Super::EndPlay(EndPlayReason);
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

	ActiveStoryWidget = CreateWidget<UUserWidget>(PC, StoryWidgetClass);
	if (ActiveStoryWidget) {
		// AddToViewport / Pause / 페이지 진행은 위젯 BP가 자체 처리
		static const FName PlaySequenceFunctionName(TEXT("PlaySequence"));
		if (UFunction* PlaySequenceFunction = ActiveStoryWidget->FindFunction(PlaySequenceFunctionName)) {
			struct FPlaySequenceParameters
			{
				UStorySequence* Story;
			};

			FPlaySequenceParameters Parameters{Definition->Story};
			ActiveStoryWidget->ProcessEvent(PlaySequenceFunction, &Parameters);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("[Story] %s does not implement PlaySequence"), *GetNameSafe(ActiveStoryWidget));
		}
	}
}
