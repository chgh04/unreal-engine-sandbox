// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemyCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "EAIState.h"
#include "BaseAIController.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BaseProjectile.h"
#include "Components/WidgetComponent.h"
#include "BaseEnemyWidget.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"  
#include "Perception/AIPerceptionSystem.h"
#include "BaseCharacter.h"
#include "BaseGameMode.h"

// Sets default values
ABaseEnemyCharacter::ABaseEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HPWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HP Bar"));
	HPWidgetComponent->SetupAttachment(GetMesh());
	// 월드 스페이스 방식(캐릭터 머리 위에 체력 표시)
	HPWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	// 크기 설정
	HPWidgetComponent->SetDrawSize(FVector2D(150.f, 12.f));
	// 위치 오프셋
	HPWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 100.f));

	// 퍼셉션 인지소스 생성(AI 아군 및 적 구분)
	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimulusSource"));
	StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
	StimuliSource->bAutoRegister = true;
}

// Called when the game starts or when spawned
void ABaseEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentHP = MaxHP;

	if (HPWidgetClass)
	{
		HPWidgetComponent->SetWidgetClass(HPWidgetClass);
	}
}

void ABaseEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	PossessAIController = Cast<ABaseAIController>(NewController);
	if (PossessAIController)
	{	
		bIsPossessed = true;
		//if (bIsPossessed) UE_LOG(LogTemp, Warning, TEXT("bIsPossessed is True"))

		// 적 캐릭터는 2번 팀
		PossessAIController->SetGenericTeamId(FGenericTeamId(2));
	}
}

// Called every frame
void ABaseEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 죽었다면 10초 후 파괴
	if (bIsDead)
	{
		DestroyActor(DeltaTime);
		return;
	}

	// AIController에 의해 Possessed 되어있지 않다면 리턴
	if (!bIsPossessed)
	{
		return;
	}

	// TODO: 캐릭터가 플레이어를 바라보는 각도
	if (PossessAIController->GetCurrentTarget())
	{
		YawDiff = YawDifference();
	}

	// 캐릭터 보간 회전 로직, PossessAIController는 굳이 인자로 주지 않아도 됨(로컬 변수로 선언되어있기 때문)
	RotateCheck(DeltaTime, PossessAIController);

	// 캐릭터 이동속도 변경 로직
	ChangeToCombatSpeed(PossessAIController);

	// 캐릭터 Patrol->Idle 변경 로직
	PatrolCount(DeltaTime, PossessAIController);
	
	// 거리에 따른 위젯 표시 로직
	ShowWidgetByDistance();
}

// Called to bind functionality to input
void ABaseEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseEnemyCharacter::CallAttackEnd()
{
	OnAttackEnd.Broadcast();
}

void ABaseEnemyCharacter::AttackTrace()
{
	if (!GetMesh())
	{
		return;
	}

	FHitResult OutHit;

	// 박스 트레이스 시작/종료 소켓
	const FName SocketStart = TEXT("WeaponStart");
	const FName SocketEnd = TEXT("WeaponEnd");

	if (!GetMesh()->DoesSocketExist(SocketStart) || !GetMesh()->DoesSocketExist(SocketEnd))
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon sockets not found - Enemy"));
		return;
	}

	// 소켓의 위치
	FVector Start = GetMesh()->GetSocketLocation(SocketStart);
	FVector End = GetMesh()->GetSocketLocation(SocketEnd);

	FVector BoxHalfSize(20.0f, 20.0f, 20.0f);

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

	OnHit(OutHit);
}

void ABaseEnemyCharacter::ResetList()
{
	DamagedActorList.Empty();
}

void ABaseEnemyCharacter::OnHit(FHitResult Hit)
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

	UGameplayStatics::ApplyPointDamage(
		DamagedActor,
		BaseAD,
		Hit.TraceStart - Hit.TraceEnd,
		Hit,
		GetController(),
		this,
		UDamageType::StaticClass()
	);
}

float ABaseEnemyCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float DamageTaken = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (BaseDefense < DamageAmount)
	{
		CurrentHP -= (DamageAmount - BaseDefense);
	}

	// 타격 사운드 재생
	PlayHitSound();

	UE_LOG(LogTemp, Warning, TEXT("%s took %f Damage. Current HP: %f"), *GetName(), DamageAmount, CurrentHP);

	// 등 뒤에서 공격당할 경우
	if (PossessAIController)
	{
		if (PossessAIController->GetAIState() != EAIState::Attack)
		{	
			PossessAIController->ToggleVisionAngle(true);
			bCanRotate = true;
;		}
	}

	if (CurrentHP <= 0.f)
	{	
		UE_LOG(LogTemp, Warning, TEXT("%s Died"), *GetName());

		// 사망로직 일괄실행
		DeadSequence(DamageCauser);

		// 플레이어에게 경험치 전달
		if (EventInstigator && EventInstigator->GetPawn())
		{
			if (ABaseCharacter* Player = Cast<ABaseCharacter>(EventInstigator->GetPawn()))
			{
				Player->AddExperience(RewardXP);
			}
		}

		// 캐릭터별 사망 이벤트 실행(사망 애니메이션 등)
		OnDieEvent();
	}
	else
	{	
		// 경직 가능할때만 Hit 애니메이션이 재생됨
		if (bIsStagger)
		{
			OnDamagedEvent();
		}
	}

	// 위젯 업데이트
	if (UUserWidget* Widget = HPWidgetComponent->GetUserWidgetObject())
	{
		if (UBaseEnemyWidget* HPWidget = Cast<UBaseEnemyWidget>(Widget))
		{
			HPWidget->UpdateHP(CurrentHP / MaxHP);
		}
	}

	return DamageTaken;
}

void ABaseEnemyCharacter::DeadSequence(AActor* Target)
{	
	// 사망상태라면 재실행하지 않음 
	if (bIsDead)
	{
		return;
	}

	// AI 상태전환 
	ABaseAIController* BaseAIController = Cast<ABaseAIController>(GetController());
	if (BaseAIController)
	{
		BaseAIController->SetAIState(EAIState::Dead);
	}

	// 캐릭터가 플레이어를 바라보게 만들기
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation());
	SetActorRotation(TargetRotation);

	// 사망 플래그 활성화 및 콜리전 비활성화
	bIsDead = true;
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);	

	// 위젯 비활성화
	HPWidgetComponent->SetVisibility(false);

	// 보스일경우 게임 클리어 위젯 노출
	if (bIsStageBoss)
	{
		if (ABaseGameMode* BaseGM = Cast<ABaseGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			BaseGM->ClearGame();
		}
	}

	// Tick 비화성화
	//SetActorTickEnabled(false);
}

void ABaseEnemyCharacter::RotateToTarget(float DeltaTime, AActor* Target)
{
	if (!bCanRotate || !Target)
	{
		return;
	}

	FRotator Current = GetActorRotation();
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation());

	FRotator NewRot = FMath::RInterpTo(Current, TargetRotation, DeltaTime, 3.f);
	SetActorRotation(NewRot);
}

void ABaseEnemyCharacter::RotateCheck(float DeltaTime, ABaseAIController* BaseAIController)
{	
	if (!bCanRotate)
	{
		return;
	}

	// AIController 가져오기
	if (BaseAIController == nullptr)
	{
		return;
	}

	// AI의 타겟 가져오기, 블랙보드의 Target을 가져옴
	AActor* Target = BaseAIController->GetCurrentTarget();
	if (Target == nullptr)
	{
		return;
	}

	// 캐릭터를 공격하지 않는 도중에 보간하여 회전함
	if (bCanRotate)
	{
		RotateToTarget(DeltaTime, Target);
	}
}

void ABaseEnemyCharacter::ToggleMoveSpeed(bool bIsWalk)
{
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();

	if (bIsWalk)
	{
		MoveComp->MaxWalkSpeed = MinWalkSpeed;
	}
	else
	{
		MoveComp->MaxWalkSpeed = MaxWalkSpeed;
	}
	
}

void ABaseEnemyCharacter::ChangeToCombatSpeed(ABaseAIController* BaseAIController)
{	
	// 전투중에만 해당 로직 실행
	if (BaseAIController == nullptr || BaseAIController->GetAIState() != EAIState::Attack)
	{
		return;
	}

	// AI의 타겟 가져오기, 블랙보드의 Target을 가져옴
	AActor* Target = BaseAIController->GetCurrentTarget();
	if (Target == nullptr)
	{
		return;
	}

	float Distance = FVector::DistSquared(GetActorLocation(), Target->GetActorLocation());

	if (Distance <= FMath::Square(CombatModeDistance))
	{
		ToggleMoveSpeed(true);	// Walk
		//UE_LOG(LogTemp, Warning, TEXT("Distance: %f / Toggle Move Speed (Distance <= CombatModeDistance)"), FMath::Sqrt(Distance));
	}
	else
	{
		ToggleMoveSpeed(false);	// Run
		//UE_LOG(LogTemp, Warning, TEXT("Distance: %f / Toggle Move Speed (Distance > CombatModeDistance)"), FMath::Sqrt(Distance));
	}
}

void ABaseEnemyCharacter::PatrolCount(float DeltaTime, ABaseAIController* BaseAIController)
{	
	// Patrol 상태가 아니라면 Return
	if (BaseAIController->GetAIState() != EAIState::Patrol)
	{	
		CurrentPatrolTime = 0;
		return;
	}

	if (CurrentPatrolTime > PatrolTime)
	{
		UE_LOG(LogTemp, Warning, TEXT("PatrolCount Return, Change State to Idle"));

		BaseAIController->SetAIState(EAIState::Idle);
		return;
	}

	CurrentPatrolTime += DeltaTime;
	//UE_LOG(LogTemp, Warning, TEXT("CurrentPatrolTime: %.4f"), CurrentPatrolTime);
	

	return;
}

float ABaseEnemyCharacter::YawDifference()
{	
	if (!PossessAIController)
	{
		return 180;
	}
	
	AActor* Player = PossessAIController->GetCurrentTarget();
	if (!Player)
	{
		return 180;
	}
	
	FRotator CurrentRot = PossessAIController->GetControlRotation();
	FRotator DesireRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Player->GetActorLocation());

	float CharacterTargetYawDiff = UKismetMathLibrary::NormalizedDeltaRotator(DesireRot, CurrentRot).Yaw;
	
	//UE_LOG(LogTemp, Warning, TEXT("Yaw Differnece: %.4f"), YawDiff);
	return FMath::Abs(CharacterTargetYawDiff);
}

void ABaseEnemyCharacter::SpawnProjectile()
{
	if (!ProjectileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s tried to shoot but has no projectile set!"), *GetName());
		return;
	}

	//FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	FVector SpawnLocation = GetMesh()->GetSocketLocation("MagickSocket");
	FRotator SpawnRotation = GetControlRotation();

	// 투사체가 피해를 입힐때(ApplyPointDamage) 전달할 정보를 지정
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	ABaseProjectile* Projectile = GetWorld()->SpawnActor<ABaseProjectile>(
		ProjectileClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);
	Projectile->TargetActor = PossessAIController->GetCurrentTarget();

	if (Projectile)
	{
		//UE_LOG(LogTemp, Warning, TEXT("%s Spawned Projectile: %s"), *GetName(), *Projectile->GetName());
	}
}

void ABaseEnemyCharacter::ShowWidgetByDistance()
{	
	// AIController 없다면 리턴
	if (!PossessAIController)
	{
		return;
	}

	// 플레이어가 없다면 리턴
	APawn* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!Player)
	{
		return;
	}

	float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
	if (Distance > 1500.f)
	{
		HPWidgetComponent->SetVisibility(false);
	}
	else
	{
		HPWidgetComponent->SetVisibility(true);
	}
}

FGenericTeamId ABaseEnemyCharacter::GetGenericTeamId() const
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

void ABaseEnemyCharacter::DestroyActor(float DeltaTime)
{
	DestroyTimer += DeltaTime;

	//UE_LOG(LogTemp, Warning, TEXT("%s Died, DestroyTimer: %.2f"), *GetName(), DestroyTimer);

	if (DestroyTimer > 10.0f)
	{
		Destroy();
	}
}

void ABaseEnemyCharacter::PlayHitSound()
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			HitSound,
			GetActorLocation()
		);
	}
}