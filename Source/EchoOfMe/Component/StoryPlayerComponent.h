// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StoryPlayerComponent.generated.h"


class UMemoryFragmentDefinition;
class UMemoryComponent;
class UStoryWidget;


/*	[스토리 컷 재생기]
	기억조각 회수음이 끝나면 해당 조각의 스토리 컷을 위젯으로 재생	*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ECHOOFME_API UStoryPlayerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UStoryPlayerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 회수음 종료 신호 수신 >> 스토리 컷 재생
	UFUNCTION()
	void HandleFlashbackFinished(const UMemoryFragmentDefinition* Definition);

	// 재생할 스토리 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category="Story")
	TSubclassOf<UStoryWidget> StoryWidgetClass;

private:
	// 현재 떠 있는 스토리 위젯 >> 중복 방지
	UPROPERTY()
	TObjectPtr<UStoryWidget> ActiveStoryWidget;

	// 바인딩한 MemoryComponent 참조 >> EndPlay해제
	UPROPERTY()
	TObjectPtr<UMemoryComponent> BoundMemory;
};
