// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ListeningComponent.generated.h"


// 듣기 상태 변화 방송
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnListeningChanged, bool, bIsListening);


UCLASS(ClassGroup=(EchoOfMe), meta=(BlueprintSpawnableComponent))
class ECHOOFME_API UListeningComponent : public UActorComponent {
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UListeningComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// 듣기 시작 >> 입력 Started에서 호출
	UFUNCTION(BlueprintCallable, Category = "Listening")
	void StartListening();

	// 듣기 종료 >> 입력 Completed에서 호출
	UFUNCTION(BlueprintCallable, Category = "Listening")
	void StopListening();

	// 듣기 상태 반환
	UFUNCTION(BlueprintPure, Category = "Listening")
	bool IsListening() const { return bIsListening; }

	// 듣기 시작/종료 시 방송
	UPROPERTY(BlueprintAssignable, Category = "Listening")
	FOnListeningChanged OnListeningChanged;

private:
	bool bIsListening = false;
};
