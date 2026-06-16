// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StoryPlayerComponent.generated.h"


class UMemoryFragmentDefinition;
class UMemoryComponent;
class UStoryWidget;
class UStorySequence;
class AEndingDoorActor;
enum class EEndingType : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStoryFinished);


/*	[스토리 컷 재생기]
	- 기억조각 회수음 종료 → 해당 조각의 스토리 컷
	- 엔딩 문 열림 → 해당 엔딩 컷
	두 경로 모두 PlaySequence를 위젯에 위임	*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ECHOOFME_API UStoryPlayerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UStoryPlayerComponent();

	/*	[외부 진입점] 스토리 시퀀스 재생
		엔딩 오케스트레이션 등에서 직접 호출 가능	*/
	UFUNCTION(BlueprintCallable, Category = "Story")
	void PlayStory(UStorySequence* Sequence);

	UPROPERTY(BlueprintAssignable, Category = "Story")
	FOnStoryFinished OnStoryFinished;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 회수음 종료 신호 수신 >> 스토리 컷 재생
	UFUNCTION()
	void HandleFlashbackFinished(const UMemoryFragmentDefinition* Definition);

	// 엔딩 문 열림 신호 수신 >> 엔딩 컷 재생
	UFUNCTION()
	void HandleEndingDoorOpened(EEndingType Ending, UStorySequence* EndingStory);

	// 재생할 스토리 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category="Story")
	TSubclassOf<UStoryWidget> StoryWidgetClass;

	UFUNCTION()
	void HandleActiveStoryFinished();

	// 레벨 시작 시 1회 재생할 프롤로그 컷 (비워두면 재생 안 함)
	UPROPERTY(EditDefaultsOnly, Category = "Story")
	TObjectPtr<UStorySequence> PrologueStory;

	// 프롤로그 재생까지 지연 시간 (BeginPlay 경합 회피)
	UPROPERTY(EditDefaultsOnly, Category = "Story", meta = (ClampMin = "0.0"))
	float PrologueDelay = 0.1f;

private:
	// 레벨의 엔딩 문들을 찾아 OnEndingDoorOpened 구독
	void BindEndingDoors();

	// 현재 떠 있는 스토리 위젯 >> 중복 방지
	UPROPERTY()
	TObjectPtr<UStoryWidget> ActiveStoryWidget;

	// 바인딩한 MemoryComponent 참조 >> EndPlay해제
	UPROPERTY()
	TObjectPtr<UMemoryComponent> BoundMemory;

	// 바인딩한 엔딩 문들 >> EndPlay 해제
	UPROPERTY()
	TArray<TObjectPtr<AEndingDoorActor>> BoundDoors;

	void PlayPrologue();

	FTimerHandle PrologueTimerHandle;
};
