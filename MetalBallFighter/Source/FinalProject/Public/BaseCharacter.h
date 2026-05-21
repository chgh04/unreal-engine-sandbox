// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "LevelProgressingData.h"
#include "BaseCharacter.generated.h"

UCLASS()
class FINALPROJECT_API ABaseCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float MaxWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float MinWalkSpeed;

	// 플레이어 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float MaxHP;

	// 플레이어 공격력
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Status")
	float BaseAD;

	// 플레이어 방어력
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Status")
	float BaseDefense;

	// 플레이어의 콤보 단계
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Status")
	int32 ComboIndex;

	// 플레이어 캐릭터의 공격 트레이스, 오른쪽 손
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void AttackTrace_Right();

	// 플레이어 캐릭터의 공격 트레이스, 왼쪽 손
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void AttackTrace_Left();

	// 플레이어 캐릭터의 공격 트레이스, 오른쪽 발
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void AttackTrace_HeavyRight();

	// 플레이어 캐릭터의 공격 트레이스, 왼쪽 발
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void AttackTrace_HeavyLeft();

	// 피해 대상에게 데미지 부여
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void OnHit(FHitResult Hit, float DamageAmout = 10.f);

	// 피해 대상 배열 초기화, 블루프린트에서 호출
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ResetList();

	// 최종 공격력 보너스
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Status")
	float PlayerBonusDamage = 1.f;

	// AI Perception 감지 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class UAIPerceptionStimuliSourceComponent* StimuliSource;

	// 플레이어 캐릭터의 레벨 구조체
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	FLevelProgressingData LevelData;

	// 캐릭터 레벨 업
	UFUNCTION(BlueprintCallable, Category = "Item")
	void LevelUp();

	// 플레이어 회복 아이템 사용
	UFUNCTION(BlueprintCallable, Category = "Item")
	void UseHealingItem();

	// 플레이어 회복 아이템 회복량
	UPROPERTY(EditAnywhere, Category = "Status")
	float HealAmount = 30.f;

	// 플레이어 레벨업 사운드
	UPROPERTY(EditAnywhere, Category = "Status")
	USoundBase* LevelUpSound;

	// 플레이어 아이템 획득 사운드
	UPROPERTY(EditAnywhere, Category = "Status")
	USoundBase* ItemGetSound;

	// 플레이어 사운드 재생
	UFUNCTION()
	void PlayPlayerSound(USoundBase* PlaySound);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 플레이어 공격력 계수 반환, ComboIdx: 콤보 단계, BaseDmg: 공격 모션의 기본 공격계수, BonusDmg: 최종 대미지 보너스
	float AttackDamageScaling(int ComboIdx, float BaseDmg, float BonusDmg);

	// 팀 아이디 부여 로직 인터페이스 구현
	virtual FGenericTeamId GetGenericTeamId() const override;

	// 플레이어 캐릭터 경험치 추가
	UFUNCTION(BlueprintCallable)
	void AddExperience(int32 XP);

	// 플레이어 캐릭터 현재 체력 반환
	UFUNCTION(BlueprintCallable)
	float GetCurrentHP() const;

	// 플레이어 캐릭터 최대 체력 반환
	UFUNCTION(BlueprintCallable)
	float GetMaxHP() const;

	// 플레이어 캐릭터 현재 경험치 반환
	int32 GetCurrentEXP() const;

	// 플레이어 캐릭터의 필요 경험치 반환
	int32 GetRequiredEXP() const;

	// 플레이어 레벨 반환
	int32 GetPlayerLevel() const;

	// 회복 아이템 추가
	UFUNCTION(BlueprintCallable)
	void AddHealingItem(int32 Amount);

	// 회복 아이템 개수 반환
	UFUNCTION(BlueprintCallable)
	int32 GetHealingItemCount() const;

private:
	// 피해대상 저장 리스트(중복타격 방지)
	TArray<AActor*> DamagedActorList;

	// 트레이스 실행 함수 
	void TraceFunc(FVector Start, FVector End, float Damage);

	// 플레이어의 현재 체력
	float CurrentHP;

	// TakeDamage 함수 구현
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// 회복 아이템 개수
	int32 HealingItemCount = 2;
	
// UI 업데이트용 델리게이트 선언
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnExperienceChanged, int32, CurrentXP, int32, RequiredXP);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, CurrentHP, float, MaxHP);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelUp, int32, NewLevel);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealingItemChanged, int32, NewCount);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnExperienceChanged OnExperienceChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnLevelUp OnLevelUp;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealingItemChanged OnHealingItemChanaged;
};
