// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EAIState.h"
#include "BaseAIController.generated.h"

/**
 * AI Perception 생성(시각 등 감지센서 등록)
 * Black Board와 Behavior Tree와의 직접적 연동
 */
UCLASS()
class FINALPROJECT_API ABaseAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABaseAIController();

protected:
	virtual void BeginPlay() override;

	// AIController가 Pawn을 Possess할 때 호출됨
	virtual void OnPossess(APawn* InPawn) override;

public:
	// AI Perception 생성
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class UAIPerceptionComponent* AIPerception;

	class UAISenseConfig_Sight* SightConfig;

	// Black Board 및 Behavior Tree와 연동을 위해 선언(에디터에서 드롭다운 선택)
	UPROPERTY(EditAnywhere, Category = "AI")
	UBlackboardData* BBAsset;

	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* BTAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")	// AIController의 기본 UBlackboardComponent를 활용 
	UBlackboardComponent* BlackboardComp;
	
	// 캐릭터 상태전환
	void SetAIState(EAIState NewState);

	// 캐릭터 상태를 EAIState로 반환
	EAIState GetAIState();

	// 시야에 플레이어 발견시 실행
	UFUNCTION()
	void OnTargetDetecting(AActor* Actor, FAIStimulus Stimuls);

	// 블랙보드의 TargetActor를 인자로 설정
	UFUNCTION()
	void SetCurrentTarget(AActor* NewTarget);

	// 현재 블랙보드의 TargetActor를 반환, 적 캐릭터 C++클래스에서의 회전, 속도 조절을 위해 사용함
	UFUNCTION()
	AActor* GetCurrentTarget();

	// 캐릭터의 시야를 180도/90도 토글
	UFUNCTION()
	void ToggleVisionAngle(bool bIsInCombat);

	// BB의 OriginLocation을 현재 위치로 설정
	void SetOriginLocation(FVector CharacterLocation);

private:
	// 블랙보드의 AI 상태 접근을 위한 FName, 블랙보드 키의 이름
	FName BBKey_AIState = TEXT("AIState");
	FName BBKey_TargetActor = TEXT("TargetActor");
	FName BBKey_PatrolLocation = TEXT("PatrolLocation");
	FName BBKey_OriginLocation = TEXT("OriginLocation");

	// 캐릭터의 현재 상태 저장
	EAIState CurrentState;
};
