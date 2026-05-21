// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "BaseEnemyCharacter.generated.h"

UCLASS()
class FINALPROJECT_API ABaseEnemyCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// PossessedBy Override
	virtual void PossessedBy(AController* NewController) override;

	// 캐릭터 이동속도 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float MaxWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float MinWalkSpeed;

	// 캐릭터의 공격간 딜레이
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float AttackDelay;

	// 캐릭터 공격력
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Status")
	float BaseAD;

	// 캐릭터 방어력
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Status")
	float BaseDefense;

	// 캐릭터의 경직 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
	bool bIsStagger = true;

	// 캐릭터가 전투시(플레이어와 접근시) 이동속도 감소가 발생하는 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float CombatModeDistance;

	// 적 캐릭터의 제공 경험치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	int32 RewardXP;

	// 캐릭터의 HP바를 표시할 위젯
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	class UWidgetComponent* HPWidgetComponent;

	// 위젯 클래스 저장 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UBaseEnemyWidget> HPWidgetClass;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEnd);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAttackEnd OnAttackEnd;

	// 적 캐릭터 공격 트레이스 
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void AttackTrace();

	// 대미지 전달 함수 
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void OnHit(FHitResult Hit);

	// 캐릭터별 피격 이벤트, 블루프린트 구현
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Combat")
	void OnDamagedEvent();

	// 캐릭터별 사망 이벤트, 블루프린트 구현
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Combat")
	void OnDieEvent();

	// 캐릭터를 타겟으로 회전
	void RotateToTarget(float DeltaTime, AActor* Target);

	// 캐릭터의 이동속도를 최소/최대 이동속도로 변경
	UFUNCTION(BlueprintCallable, Category = "Status")
	void ToggleMoveSpeed(bool bIsWalk);

	// 캐릭터가 플레이어와 가깝다면 이동속도를 최소 속도로 변환
	void ChangeToCombatSpeed(class ABaseAIController* BaseAIController);

	// 캐릭터의 순찰->비전투 상태 변경 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status")
	float PatrolTime;

	// 캐릭터의 순찰 시간 카운트 및 상태 변경
	UFUNCTION()
	void PatrolCount(float DeltaTime, class ABaseAIController* BaseAIController);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	float YawDiff;
	
	// 피해 대상 배열 초기화, 블루프린트에서 호출
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ResetList();

	// 투사체를 사용하는 캐릭터만 해당 변수 사용, 발사 할 투사체
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status")
	TSubclassOf<class ABaseProjectile> ProjectileClass;

	// 퍼셉션 인지 소스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class UAIPerceptionStimuliSourceComponent* StimuliSource;

	// 타격 사운드
	UPROPERTY(EditAnywhere, Category = "Status")
	USoundBase* HitSound;

	// 타격 사운드 재생
	UFUNCTION()
	void PlayHitSound();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 이동 접근 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float AcceptanceRadius;

	// 공격 이벤트, 자식BP에서 구현
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "AI")
	void AttackEvent(AActor* Actor);

	// 공격 종료를 브로드캐스팅
	UFUNCTION(BlueprintCallable)
	void CallAttackEnd();

	UPROPERTY(EditDefaultsOnly, Category = "Status")
	float MaxHP;

	// 캐릭터가 죽었는지 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	bool bIsDead = false;
	
	// 캐릭터 회전 허용
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Status")
	bool bCanRotate = false;

	// 캐릭터의 patrol 범위
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Status")
	float PatrolRadius;

	// 투사체를 사용하는 캐릭터만 해당 함수 사용, 투사체 발사
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SpawnProjectile();

	// 퍼셉션 팀 아이디 가져오기
	virtual FGenericTeamId GetGenericTeamId() const override;

	// 스테이지 보스캐릭터인지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	bool bIsStageBoss = false;

private:
	float CurrentHP;

	// TakeDamage 함수 구현
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// 캐릭터 사망시 로직 일괄실행 
	void DeadSequence(AActor* Target);

	// AIController
	class ABaseAIController* PossessAIController;

	// 캐릭터가 AIController에 Possessed 되어있는지 판단
	bool bIsPossessed = false;

	// 캐릭터 회전 판단 
	void RotateCheck(float DeltaTime, class ABaseAIController* BaseAIController);

	// 캐릭터가 Patrol 상태로 바뀐 후로부터의 시간
	float CurrentPatrolTime;

	// 캐릭터와 타겟를 바라보는 각도 확인
	float YawDifference();

	// 피해대상 저장 리스트(중복타격 방지)
	TArray<AActor*> DamagedActorList;

	// 위젯의 거리에 따른 표시 지정 함수
	void ShowWidgetByDistance();

	// 액터 파괴 함수
	void DestroyActor(float DeltaTime);

	// 액터 파괴 타이머
	float DestroyTimer = 0;
};
