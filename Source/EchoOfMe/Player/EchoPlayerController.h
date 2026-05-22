// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EchoPlayerController.generated.h"

class UInputMappingContext;
class AEchoPlayerCharacter;

UCLASS(Abstract)
class ECHOOFME_API AEchoPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	// 기본 입력 매핑 컨텍스트 배열
	UPROPERTY(EditAnywhere, Category = "Input|Input Mapping")
	TArray<UInputMappingContext*> DefaultMappingContexts;
	// 리스폰할 플레이어 캐릭터
	UPROPERTY(EditAnywhere, Category = "Respawn")
	TSubclassOf<AEchoPlayerCharacter> CharacterClass;
	//다시 스폰 할 때 속성
	FTransform RespawnTransform;
public:
	// 게임 시작 이벤트 메소드
	virtual void BeginPlay() override;
	// 입력맵핑 컨텍스트 등록 수행 이벤트 메소드
	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* InPawn) override;
	// 리스폰 위치 업데이트
	void SetRespawnTransform(const FTransform& NewRespawn);

	UFUNCTION()
	void OnPawnDestroyed(AActor* DestroyActor);


};
