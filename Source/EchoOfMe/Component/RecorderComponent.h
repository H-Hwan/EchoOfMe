// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RecorderComponent.generated.h"


class UInventoryItemDefinition;
class URecorderItemDefinition;
class UAudioComponent;

// 재생 종료
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRecorderFinished);
// 단계 재생 시작
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRecorderStagePlayed, int32, Stage);


UCLASS( ClassGroup=(EchoOfMe), meta=(BlueprintSpawnableComponent) )
class ECHOOFME_API URecorderComponent : public UActorComponent {
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URecorderComponent();

public:
	// 현재 단계의 음성 재생
	UFUNCTION(BlueprintCallable, Category = "Recorder")
	void PlayCurrentStage();

	// 재생 중인 음성 중단
	UFUNCTION(BlueprintCallable, Category = "Recorder")
	void StopPlayBack();

	// 인식 단계 한 칸 진행 >> 스토리 트리거 호출
	UFUNCTION(BlueprintCallable, Category = "Recorder")
	void AdvanceStage();

	// 인식 단계 반환
	UFUNCTION(BlueprintPure, Category = "Recorder")
	int32 GetPlaybackStage() const { return PlaybackStage; }

	// 재생 중 여부 반환
	bool IsPlaying() const;

	UPROPERTY(BlueprintAssignable, Category = "Recorder")
	FOnRecorderFinished OnRecorderFinished;
	UPROPERTY(BlueprintAssignable, Category = "Recorder")
	FOnRecorderStagePlayed OnRecorderStagePlayed;
protected:
	virtual void BeginPlay() override;

	// 인벤토리에서 아이템 사용되면 호출 >> 녹음기일 때만
	UFUNCTION()
	void HandleItemUsed(UInventoryItemDefinition* Item);

	// 음성 재생이 끝났을 때 콜백
	UFUNCTION()
	void HandleAudioFinished();

	// 녹음기 정의
	UPROPERTY(EditDefaultsOnly, Category = "Recorder")
	TObjectPtr<URecorderItemDefinition> RecorderDefinition;

private:
	// 현재 인식 단계 >> 추후 GameManager의 RecorderPlaybackCount로 이전 예정
	int32 PlaybackStage = 0;

	// 현재 재생 중인 음성 >> 중복 재생 방지 및 중단에 사용
	UPROPERTY()
	TObjectPtr<UAudioComponent> ActiveAudio;
};
