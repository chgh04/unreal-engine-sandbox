// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"


ABaseAIController::ABaseAIController()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	SightConfig->SightRadius = 1000.f;
	SightConfig->LoseSightRadius = 1300.f;
	SightConfig->PeripheralVisionAngleDegrees = 90.f;
	//SightConfig->SetMaxAge(5.f);
	
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	AIPerception->ConfigureSense(*SightConfig);

	AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());
}


void ABaseAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BBAsset == nullptr)
	{
		return;
	}

	// UE_LOG(LogTemp, Warning, TEXT("AIController Possessed"));

	AIPerception->OnTargetPerceptionUpdated.AddDynamic(
		this, &ABaseAIController::OnTargetDetecting
	);

	if (UseBlackboard(BBAsset, BlackboardComp))
	{
		RunBehaviorTree(BTAsset);

		SetAIState(EAIState::Idle);

		// Possess 될 때의 위치를 OriginLocation으로 지정
		SetOriginLocation(InPawn->GetActorLocation());
	}
}

void ABaseAIController::OnTargetDetecting(AActor* Actor, FAIStimulus Stimuls)
{	
	// Dead 상태중에 상태를 바꿀 수 있는 버그? 있음 -> 캐릭터 회전이 Character Movement를 쓰지 않아서? 인것으로 추측됨 왠진몰라~~
	if (CurrentState == EAIState::Dead)	// 사망상태라면 리턴
	{
		return;
	}

	if (Stimuls.WasSuccessfullySensed())
	{
		UE_LOG(LogTemp, Warning, TEXT("Detected: %s"), *Actor->GetName());

		// 블랙보드의 TargetActor에 발견한 적 등록
		SetCurrentTarget(Actor);

		// 블랙보드 AIState 동기화
		SetAIState(EAIState::Attack);

		// 적 캐릭터의 인식범위를 180도로 바꾸기
		ToggleVisionAngle(true);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Lost: %s"), *Actor->GetName());

		// 순찰 위치를 마지막으로 플레이어를 인식한 장소로 지정
		BlackboardComp->SetValueAsVector(BBKey_PatrolLocation, Actor->GetActorLocation());
		
		// 블랙보드의 TargetActor 클리어
		BlackboardComp->ClearValue(BBKey_TargetActor);

		// 블랙보드 AIState 동기화
		SetAIState(EAIState::Patrol);

		// 적 캐릭터의 인식범위를 90도로 바꾸기
		ToggleVisionAngle(false);
	}
}

void ABaseAIController::SetAIState(EAIState NewState)	
{
	if (CurrentState == NewState)
	{
		return;
	}

	CurrentState = NewState;

	if (BlackboardComp && BBAsset)
	{
		BlackboardComp->SetValueAsEnum(BBKey_AIState, static_cast<uint8>(NewState));
	}
}

EAIState ABaseAIController::GetAIState()
{
	return CurrentState;
}

void ABaseAIController::SetCurrentTarget(AActor* NewTarget)
{
	if (BlackboardComp && BBAsset)
	{
		BlackboardComp->SetValueAsObject(BBKey_TargetActor, Cast<AActor>(NewTarget));
	}
}

AActor* ABaseAIController::GetCurrentTarget()
{
	if (BlackboardComp && BBAsset)
	{
		return Cast<AActor>(BlackboardComp->GetValueAsObject(BBKey_TargetActor));
	}
	return nullptr;
}

void ABaseAIController::ToggleVisionAngle(bool bIsInCombat)
{
	if (SightConfig)
	{
		SightConfig->PeripheralVisionAngleDegrees = bIsInCombat ? 180.f : 90.f;

		AIPerception->RequestStimuliListenerUpdate();
	}
}

void ABaseAIController::SetOriginLocation(FVector CharacterLocation)
{
	if (BlackboardComp && BBAsset)
	{	
		//UE_LOG(LogTemp, Warning, TEXT("Origin Location Is Set"));
		BlackboardComp->SetValueAsVector(BBKey_OriginLocation, CharacterLocation);
	}
}