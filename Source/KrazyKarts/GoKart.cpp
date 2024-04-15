// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKart.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;	//서버와 클라이언트에 동기화

}

// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay();	

	if (HasAuthority())
	{
		NetUpdateFrequency = 1;	//서버에서 클라이언트로 전송하는 빈도
	}
}

void AGoKart::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGoKart, ServerState);
}

FString GetEnumText(ENetRole Role)
{
	switch (Role)
	{
	case ROLE_None:
		return "None";
	case ROLE_SimulatedProxy:
		return "SimulatedProxy";
	case ROLE_AutonomousProxy:
		return "AutonomousProxy";
	case ROLE_Authority:
		return "Authority";
	default:
		return "Error";
	}
}

// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled())
	{
		FGoKartMove Move = CreateMove(DeltaTime);

		if(!HasAuthority())
			UnacknowledgedMoves.Add(Move);

		UE_LOG(LogTemp, Warning, TEXT("Queue length: %d"), UnacknowledgedMoves.Num());
		Server_SendMove(Move);
		SimulateMove(Move);
	}

	DrawDebugString(GetWorld(), FVector(0, 0, 100), GetEnumText(GetLocalRole()), this, FColor::White, DeltaTime);
}

void AGoKart::SimulateMove(const FGoKartMove& Move)
{
	FVector Force = GetActorForwardVector() * MaxDrivingForce * Move.ThrottleValue;
	Force += GetAirResistance();	// 공기 저항 계산
	Force += GetRollingResistance();

	FVector Acceleration = Force / Mass;	// a = F/m (F = 힘, m = 질량)
	Velocity += Acceleration * Move.DeltaTime;	// v = u + at (u = 처음 속도, a = 가속도, t = 시간)		

	ApplyRotation(Move.DeltaTime, Move.SteeringThrow);
	UpdateLocationFromVelocity(Move.DeltaTime);	//속도에 따른 위치 업데이트	
}

FGoKartMove AGoKart::CreateMove(float DeltaTime)
{
	FGoKartMove Move;
	Move.DeltaTime = DeltaTime;
	Move.SteeringThrow = SteeringThrow;
	Move.ThrottleValue = ThrottleValue;
	Move.Time = GetWorld()->TimeSeconds;
	return Move;
}

void AGoKart::ClearAcknowledgedMoves(FGoKartMove LastMove)
{
	TArray<FGoKartMove> NewMoves;
	for (const FGoKartMove& Move : UnacknowledgedMoves)
	{
		if (Move.Time > LastMove.Time)
		{
			NewMoves.Add(Move);
		}
	}
	UnacknowledgedMoves = NewMoves;
}

/// <summary>
/// 공기 저항 계산
/// </summary>
/// <returns></returns>
FVector AGoKart::GetAirResistance()
{
	return -Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient;	// F = -v^2 * DragCoefficient
}

FVector AGoKart::GetRollingResistance()
{
	float AccelerationDueToGravity = -GetWorld()->GetGravityZ() / 100;	//중력 가속도 m/s^2로 변환
	float NormalForce = Mass * AccelerationDueToGravity;	// N = m * g
	return -Velocity.GetSafeNormal() * RollingResistanceCoefficient * NormalForce;	// F = -N * RollingResistanceCoefficient
}

void AGoKart::ApplyRotation(float DeltaTime, float SteeringThrowValue)
{
	float DeltaLocation = FVector::DotProduct(GetActorForwardVector(), Velocity) * DeltaTime;	// v = s/t (v = 속도, s = 거리, t = 시간)
	float RotationAngle = DeltaLocation / MinTurningRadius * SteeringThrowValue;	// s = r * theta (r = 반지름, theta = 각도)
	FQuat RotationDelta(GetActorUpVector(), RotationAngle);	//회전 각도 계산

	Velocity = RotationDelta.RotateVector(Velocity);	//속도 벡터에 회전 적용	
	AddActorWorldRotation(RotationDelta);	//회전 적용
}

void AGoKart::UpdateLocationFromVelocity(float DeltaTime)
{
	FVector Translation = Velocity * 100 * DeltaTime;	// cm/s -> m/s 변환

	FHitResult HitResult;
	AddActorWorldOffset(Translation, true, &HitResult);

	//충돌 처리
	if (HitResult.IsValidBlockingHit())
	{
		Velocity = FVector::ZeroVector;
	}
}

// Called to bind functionality to input
void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// throttle 
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &AGoKart::Throttle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &AGoKart::Throttle);

		// steering 
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &AGoKart::Steering);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &AGoKart::Steering);
	}
}

void AGoKart::Throttle(const FInputActionValue& Value)
{	
	ThrottleValue = Value.Get<float>();
}

void AGoKart::Steering(const FInputActionValue& Value)
{
	SteeringThrow = Value.Get<float>();
}

void AGoKart::OnRep_ServerState()
{
	SetActorTransform(ServerState.Transform);	//위치 동기화
	Velocity = ServerState.Velocity;	//속도 동기화
	ClearAcknowledgedMoves(ServerState.LastMove);	//동기화된 이동 삭제
}

/// <summary>
/// RPC 함수
/// </summary>
/// <param name="Value"></param>
void AGoKart::Server_SendMove_Implementation(FGoKartMove Move)
{
	SimulateMove(Move);
	ServerState.LastMove = Move;
	ServerState.Transform = GetActorTransform();
	ServerState.Velocity = Velocity;
}

bool AGoKart::Server_SendMove_Validate(FGoKartMove Move)
{
	return true;
}
