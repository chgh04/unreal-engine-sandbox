// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "BaseEnemyCharacter.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"  
#include "Perception/AIPerceptionSystem.h"
#include "BaseGameMode.h"


// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimulusSource"));
	StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
	StimuliSource->bAutoRegister = true;
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentHP = MaxHP;

	// UI 브로드캐스트 (시작할때 체력바 채우기)
	OnHealthChanged.Broadcast(CurrentHP, MaxHP);
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ABaseCharacter::AttackDamageScaling(int ComboIdx, float BaseDmg, float BonusDmg)
{	
	// ((ComboIndex) * 0.25 + 1) -> 콤보 1단계(idx가 0)부터 1, 1.25, 1.5, 1.75배율의 공격
	// 약공격 BaseDmg: 1, 강공격 BaseDmg: 1.5
	float ActualDamge = (BaseAD * ((ComboIndex) * 0.25 + 1) * BaseDmg) * BonusDmg;
	
	return ActualDamge;
}

void ABaseCharacter::TraceFunc(FVector Start, FVector End, float Damage)
{
	// 트레이스 충돌 정보 
	FHitResult OutHit;

	// 트레이스 박스 사이즈 
	FVector BoxHalfSize(30.0f, 30.0f, 30.0f);

	// 인식할 오브젝트 타입
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	// 자기 자신을 무시
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	// trace 수행
	bool bHit = UKismetSystemLibrary::BoxTraceSingleForObjects(
		this,
		Start,
		End,
		BoxHalfSize,
		FRotator::ZeroRotator,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,	//ForDuration
		OutHit,
		true
	);

	// 이벤트 호출
	OnHit(OutHit, Damage);
}

void ABaseCharacter::ResetList()
{
	DamagedActorList.Empty();
}

void ABaseCharacter::AttackTrace_Right()
{
	if (!GetMesh())
	{
		return;
	}

	//UE_LOG(LogTemp, Warning, TEXT("AttackTrace_Right Called"));

	// 박스 트레이스 시작/종료 소켓
	const FName SocketStart_Right = TEXT("WeaponStart_Right");
	const FName SocketEnd_Right = TEXT("WeaponEnd_Right");

	// 소켓 존재 확인 
	if (!GetMesh()->DoesSocketExist(SocketStart_Right) || !GetMesh()->DoesSocketExist(SocketEnd_Right))
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon sockets not found - Right"));
		return;
	}

	// 소켓의 위치 저장
	FVector Start = GetMesh()->GetSocketLocation(SocketStart_Right);
	FVector End = GetMesh()->GetSocketLocation(SocketEnd_Right);

	// 피해량 지정
	float Damage = AttackDamageScaling(ComboIndex, 1, PlayerBonusDamage);

	// 트레이스 실행 
	TraceFunc(Start, End, Damage);
}

void ABaseCharacter::AttackTrace_Left()
{
	if (!GetMesh())
	{
		return;
	}

	//UE_LOG(LogTemp, Warning, TEXT("AttackTrace_Left Called"));

	// 박스 트레이스 시작/종료 소켓
	const FName SocketStart_Left = TEXT("WeaponStart_Left");
	const FName SocketEnd_Left = TEXT("WeaponEnd_Left");

	// 소켓 존재 확인 
	if (!GetMesh()->DoesSocketExist(SocketStart_Left) || !GetMesh()->DoesSocketExist(SocketEnd_Left))
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon sockets not found - Left"));
		return;
	}

	// 소켓의 위치
	FVector Start = GetMesh()->GetSocketLocation(SocketStart_Left);
	FVector End = GetMesh()->GetSocketLocation(SocketEnd_Left);

	// 피해량 지정
	float Damage = AttackDamageScaling(ComboIndex, 1, PlayerBonusDamage);

	// 트레이스 실행 
	TraceFunc(Start, End, Damage);
}

void ABaseCharacter::AttackTrace_HeavyRight()
{
	if (!GetMesh())
	{
		return;
	}

	//UE_LOG(LogTemp, Warning, TEXT("AttackTrace_HeavyRight Called"));

	// 박스 트레이스 시작/종료 소켓
	const FName SocketStart_Left = TEXT("WeaponStart_RightLeg");
	const FName SocketEnd_Left = TEXT("WeaponEnd_RightLeg");

	// 소켓 존재 확인 
	if (!GetMesh()->DoesSocketExist(SocketStart_Left) || !GetMesh()->DoesSocketExist(SocketEnd_Left))
	{
		UE_LOG(LogTemp, Warning, TEXT("Leg sockets not found - Right"));
		return;
	}

	// 소켓의 위치
	FVector Start = GetMesh()->GetSocketLocation(SocketStart_Left);
	FVector End = GetMesh()->GetSocketLocation(SocketEnd_Left);

	// 피해량 지정
	float Damage = AttackDamageScaling(ComboIndex, 1.5, PlayerBonusDamage);

	// 트레이스 실행 
	TraceFunc(Start, End, Damage);
}

void ABaseCharacter::AttackTrace_HeavyLeft()
{
	if (!GetMesh())
	{
		return;
	}

	//UE_LOG(LogTemp, Warning, TEXT("AttackTrace_HeavyLeft Called"));

	// 박스 트레이스 시작/종료 소켓
	const FName SocketStart_Left = TEXT("WeaponStart_LeftLeg");
	const FName SocketEnd_Left = TEXT("WeaponEnd_LeftLeg");

	// 소켓 존재 확인 
	if (!GetMesh()->DoesSocketExist(SocketStart_Left) || !GetMesh()->DoesSocketExist(SocketEnd_Left))
	{
		UE_LOG(LogTemp, Warning, TEXT("Leg sockets not found - Left"));
		return;
	}

	// 소켓의 위치
	FVector Start = GetMesh()->GetSocketLocation(SocketStart_Left);
	FVector End = GetMesh()->GetSocketLocation(SocketEnd_Left);

	// 피해량 지정
	float Damage = AttackDamageScaling(ComboIndex, 1.5, PlayerBonusDamage);

	// 트레이스 실행 
	TraceFunc(Start, End, Damage);
}

void ABaseCharacter::OnHit(FHitResult Hit, float DamageAmount)
{
	AActor* DamagedActor = Hit.GetActor();

	if (!DamagedActor)
	{
		return;
	}

	// 중복 방지를 위해 피해액터 배열에 액터 추가 
	if (!DamagedActorList.Contains(DamagedActor))
	{
		DamagedActorList.Add(DamagedActor);

		//UE_LOG(LogTemp, Warning, TEXT("Hit: %s added to DamagedActorList"), *DamagedActor->GetName());
	}
	else
	{
		return;
	}

	// 이미 사망한 적에게는 데미지를 주지 않음
	ABaseEnemyCharacter* EnemyActor = Cast<ABaseEnemyCharacter>(DamagedActor);
	if (EnemyActor && EnemyActor->bIsDead == false)
	{
		UGameplayStatics::ApplyPointDamage(
			DamagedActor,
			DamageAmount,
			Hit.TraceStart - Hit.TraceEnd,
			Hit,
			GetController(),
			this,
			UDamageType::StaticClass()
		);
	}
}

float ABaseCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float DamageTaken = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (BaseDefense < DamageAmount)
	{
		CurrentHP -= (DamageAmount - BaseDefense);
	}

	UE_LOG(LogTemp, Warning, TEXT("Player took % f Damage. Current HP : % f"), DamageAmount, CurrentHP);

	// 델리게이트 브로드캐스팅(UI 업데이트)
	OnHealthChanged.Broadcast(CurrentHP, MaxHP);

	if (CurrentHP <= 0.f)
	{
		if (ABaseGameMode* BaseGM = Cast<ABaseGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			BaseGM->GameOver();
		}
	}

	return DamageTaken;

	// TODO: 게임오버(플레이어 사망) 로직 필요
}

FGenericTeamId ABaseCharacter::GetGenericTeamId() const 
{
	if (const AController* Ctrl = GetController())
	{
		if (const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(Ctrl))
		{
			return TeamAgent->GetGenericTeamId();
		}
	}

	return FGenericTeamId(0); // Neutral fallback
}

void ABaseCharacter::AddExperience(int32 XP)
{	
	UE_LOG(LogTemp, Warning, TEXT("Player Get XP: %d"), XP);
	LevelData.CurrentXP += XP;

	// 델리게이트 브로드캐스팅(UI 업데이트)
	OnExperienceChanged.Broadcast(LevelData.CurrentXP, LevelData.RequiredXP);

	while (LevelData.CurrentXP >= LevelData.RequiredXP)
	{
		LevelData.CurrentXP -= LevelData.RequiredXP;
		LevelUp();
	}
}

void ABaseCharacter::LevelUp()
{
	LevelData.Level++;

	LevelData.RequiredXP = FMath::CeilToInt(LevelData.RequiredXP * LevelData.GrowthRate);

	// 플레이어 레벨 업 사운드 재생
	PlayPlayerSound(LevelUpSound);

	// 델리게이트 브로드캐스팅(UI 업데이트)
	OnLevelUp.Broadcast(LevelData.Level);
	OnExperienceChanged.Broadcast(LevelData.CurrentXP, LevelData.RequiredXP);

	// 최대체력, 공격력, 방어력 증가
	MaxHP += 10;
	BaseAD += 2;
	BaseDefense += 1;
	if ((CurrentHP += (MaxHP / 10)) > MaxHP)
	{
		CurrentHP = MaxHP;
		UE_LOG(LogTemp, Warning, TEXT("HP Full Recovered, MaxHP / 10 = %.2f"), (MaxHP / 10));
	}
	else
	{
		CurrentHP += (MaxHP / 10);
	}
	HealAmount += 5.f;

	OnHealthChanged.Broadcast(GetCurrentHP(), GetMaxHP());

	UE_LOG(LogTemp, Warning, TEXT("LEVEL UP! New Level: %d"), LevelData.Level);
}

float ABaseCharacter::GetCurrentHP() const
{
	return CurrentHP;
}

float ABaseCharacter::GetMaxHP() const
{
	return MaxHP;
}

int32 ABaseCharacter::GetCurrentEXP() const
{
	return LevelData.CurrentXP;
}

int32 ABaseCharacter::GetRequiredEXP() const
{
	return LevelData.RequiredXP;
}

int32 ABaseCharacter::GetPlayerLevel() const
{
	return LevelData.Level;
}

void ABaseCharacter::AddHealingItem(int32 Amount)
{
	HealingItemCount += Amount;

	UE_LOG(LogTemp, Warning, TEXT("Healing Item: %d"), HealingItemCount);

	OnHealingItemChanaged.Broadcast(HealingItemCount);

	// 플레이어 아이템 획득 사운드 재생
	PlayPlayerSound(ItemGetSound);
}

int32 ABaseCharacter::GetHealingItemCount() const
{
	return HealingItemCount;
}

void ABaseCharacter::UseHealingItem()
{
	if (HealingItemCount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No healing items left"));
		return;
	}

	if (GetCurrentHP() >= GetMaxHP())
	{
		UE_LOG(LogTemp, Warning, TEXT("HP is already full"));
		return;
	}

	HealingItemCount--;
	CurrentHP += HealAmount;

	if (GetCurrentHP() >= GetMaxHP())
	{
		CurrentHP = MaxHP;
	}

	OnHealthChanged.Broadcast(GetCurrentHP(), GetMaxHP());
	OnHealingItemChanaged.Broadcast(HealingItemCount);
}

void ABaseCharacter::PlayPlayerSound(USoundBase* PlaySound)
{
	if (PlaySound)
	{
		{
			UGameplayStatics::PlaySoundAtLocation(
				GetWorld(),
				PlaySound,
				GetActorLocation()
			);
		}
	}
}