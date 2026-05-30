// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MemoryComponent.generated.h"


class UMemoryFragmentDefinition;
class UAudioComponent;

// [회수 완료] >> 자막·연출
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMemoryCollected, const UMemoryFragmentDefinition*, Definition);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ECHOOFME_API UMemoryComponent : public UActorComponent {
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMemoryComponent();


public:
	// 회수 직후 호출
	UFUNCTION(BlueprintCallable, Category = "Memory")
	void HandleMemoryCollected(const UMemoryFragmentDefinition* Definition);

	// 자막 표시 시간
	UPROPERTY(EditAnywhere, Category = "Memory")
	float DefaultSubtitleDuration = 3.f;

	// 회수 완료 방송
	UPROPERTY(BlueprintAssignable, Category = "Memory")
	FOnMemoryCollected OnMemoryCollected;


private:
	// 현재 재생중인 사운드 >> 중복 재생 방지
	UPROPERTY()
	TObjectPtr<UAudioComponent> ActiveFlashback;

	// 사운드 종료 바인딩 메소드
	void HandleFlashbackFinished();
};
