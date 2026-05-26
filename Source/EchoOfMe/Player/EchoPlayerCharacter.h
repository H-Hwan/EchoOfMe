// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EchoPlayerCharacter.generated.h"

//클래스 구조체 전방선언
//1. 스프링암
//2. 카메라 컴포넌트
//3. 향상된 입력 액션
//4. 애니메이션 몽타주
//입력액션벨류 구조체
//피격만 구현

class UInputAction;
struct FInputActionValue;
class UAnimMontage;
// CombatCharacter.h 파일 상단 (전방 선언 부분)
class USpringArmComponent;

UCLASS(Abstract)
class ECHOOFME_API AEchoPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDead);


public:
	// Sets default values for this character's properties
	AEchoPlayerCharacter();
	//이동 입력 처리 관련 메소드들

	// 이동 수행 메소드
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveAction;
	// 이동 입력 바인딩 메소드
	void Move(const FInputActionValue& Value);
	// 이동 수행 메소드
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoMove(float Right, float Forward);

	// 시점 변경 수행 메소드
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LookAction;
	//  시선 변경 입력 바인딩 메소드
	void Look(const FInputActionValue& Value);
	// 시점 변경 수행 메소드
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoLook(float Yaw, float Pitch);

	//이동 모드 변경 호출 메소드 선언
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	// 플레이어 컨트롤러 빙의시 입력 바인딩 처리를 수행 하는 메소드
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 점프 관련 메소드
	UPROPERTY(EditAnywhere, Category = "Input")

	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* RunningAction;
	// 시작
	void DoJumpStart(); 
	// 점프 관련 정보리셋

	// 종료
	void DoJumpEnd();

	// 착지 메소드
	virtual void Landed(const FHitResult& Hit) override;

	UPROPERTY(EditAnywhere, Category = "status")
	float MaxSpeed = 500;


	UPROPERTY(EditAnywhere, Category = "status")
	float CurrentSpeed = 320;

	UPROPERTY(EditAnywhere, Category = "status")
	float Stamina = 20;

	bool bRunning = false;

	UFUNCTION()
	void DoRunning();

	UFUNCTION()
	void StopRunning();

	


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// 이동 입력 액션 속성들

	// 이동 입력 액션
	// 마우스 시점 입력 액션

	// 카메라 관련 속성들
	// 기본 캐릭터와 카메라의 거리

	//히트판정 메소드
	//공격의 충돌 판정 수행 메소드
	//블루프린트 정의용 이펙트 출력 메소드
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* CameraBoom;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
