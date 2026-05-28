// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EchoPlayerCharacter.generated.h"


class UInputAction;
class UAnimMontage;
class USpringArmComponent;
class UCameraComponent;

class UListeningComponent;
class UFlashlightComponent;
class UNoiseMakerComponent;

struct FInputActionValue;


UCLASS(Abstract)
class ECHOOFME_API AEchoPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDead);


public:
	// Sets default values for this character's properties
	AEchoPlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 플레이어 컨트롤러 빙의시 입력 바인딩 처리를 수행 하는 메소드
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	//---
	// 이동
public:

	// 이동 입력 바인딩 메소드
	void Move(const FInputActionValue& Value);
	// 이동 수행 메소드
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoMove(float Right, float Forward);

	UFUNCTION()
	void DoRunning();

	UFUNCTION()
	void StopRunning();

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UNoiseMakerComponent> NoiseMaker;


	//---
	// 액션
public:
	// 이동 액션
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	// 시점 변경 액션
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	// 점프 액션
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	// 달리기 액션
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> RunningAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> FlashlightAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ListenAction;


	//---
	// 시점
public:
	// 추적 카메라 컴포넌트 참조
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> CameraBoom;

	//  시선 변경 입력 바인딩 메소드
	void Look(const FInputActionValue& Value);

	// 시점 변경 수행 메소드
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoLook(float Yaw, float Pitch);

	// 아래 시선각
	UPROPERTY(EditAnywhere, Category = "Camera", meta = (ClampMin = "-89.0", ClampMax = "0.0"))
	float ViewPitchMin = -55.f;

	// 위 시선각
	UPROPERTY(EditAnywhere, Category = "Camera", meta = (ClampMin = "0.0", ClampMax = "89.0"))
	float ViewPitchMax = 50.f;


	//---
	// 점프
public:
	// 시작
	void DoJumpStart();
	
	// 종료
	void DoJumpEnd();

	// 착지 메소드
	virtual void Landed(const FHitResult& Hit) override;


	//---
	// 듣기
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UListeningComponent> Listening;

	UFUNCTION()
	void HandleListeningChanged(bool bIsListening);

	void OnListenStarted();
	void OnListenCompleted();


	//---
	// 손전등
public:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UFlashlightComponent> FlashLight;

	void OnFlashLightInput();


	//---
	// 캐릭터 상태 정보
public:
	UPROPERTY(EditAnywhere, Category = "status")
	float MaxSpeed = 500;

	UPROPERTY(EditAnywhere, Category = "status")
	float CurrentSpeed = 320;

	UPROPERTY(EditAnywhere, Category = "status")
	float Stamina = 20;

	bool bRunning = false;
};
