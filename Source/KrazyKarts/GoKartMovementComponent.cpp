// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKartMovementComponent.h"

// Sets default values for this component's properties
UGoKartMovementComponent::UGoKartMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGoKartMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UGoKartMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UGoKartMovementComponent::SimulateMove(const FGoKartMove& Move)
{
	FVector Force = GetOwner()->GetActorForwardVector() * MaxDrivingForce * Move.ThrottleValue;
	Force += GetAirResistance();	// 공기 저항 계산
	Force += GetRollingResistance();

	FVector Acceleration = Force / Mass;	// a = F/m (F = 힘, m = 질량)
	Velocity += Acceleration * Move.DeltaTime;	// v = u + at (u = 처음 속도, a = 가속도, t = 시간)		

	ApplyRotation(Move.DeltaTime, Move.SteeringThrow);
	UpdateLocationFromVelocity(Move.DeltaTime);	//속도에 따른 위치 업데이트	
}

FGoKartMove UGoKartMovementComponent::CreateMove(float DeltaTime)
{
	FGoKartMove Move;
	Move.DeltaTime = DeltaTime;
	Move.SteeringThrow = SteeringThrow;
	Move.ThrottleValue = ThrottleValue;
	Move.Time = GetWorld()->TimeSeconds;
	return Move;
}

/// <summary>
/// 공기 저항 계산
/// </summary>
/// <returns></returns>
FVector UGoKartMovementComponent::GetAirResistance()
{
	return -Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient;	// F = -v^2 * DragCoefficient
}

FVector UGoKartMovementComponent::GetRollingResistance()
{
	float AccelerationDueToGravity = -GetWorld()->GetGravityZ() / 100;	//중력 가속도 m/s^2로 변환
	float NormalForce = Mass * AccelerationDueToGravity;	// N = m * g
	return -Velocity.GetSafeNormal() * RollingResistanceCoefficient * NormalForce;	// F = -N * RollingResistanceCoefficient
}

void UGoKartMovementComponent::ApplyRotation(float DeltaTime, float SteeringThrowValue)
{
	float DeltaLocation = FVector::DotProduct(GetOwner()->GetActorForwardVector(), Velocity) * DeltaTime;	// v = s/t (v = 속도, s = 거리, t = 시간)
	float RotationAngle = DeltaLocation / MinTurningRadius * SteeringThrowValue;	// s = r * theta (r = 반지름, theta = 각도)
	FQuat RotationDelta(GetOwner()->GetActorUpVector(), RotationAngle);	//회전 각도 계산

	Velocity = RotationDelta.RotateVector(Velocity);	//속도 벡터에 회전 적용	
	GetOwner()->AddActorWorldRotation(RotationDelta);	//회전 적용
}

void UGoKartMovementComponent::UpdateLocationFromVelocity(float DeltaTime)
{
	FVector Translation = Velocity * 100 * DeltaTime;	// cm/s -> m/s 변환

	FHitResult HitResult;
	GetOwner()->AddActorWorldOffset(Translation, true, &HitResult);

	//충돌 처리
	if (HitResult.IsValidBlockingHit())
	{
		Velocity = FVector::ZeroVector;
	}
}

