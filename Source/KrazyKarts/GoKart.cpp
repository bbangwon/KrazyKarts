// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKart.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay();
	
}



// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Force = GetActorForwardVector() * MaxDrivingForce * ThrottleValue;	
	Force += GetAirResistance();	// ���� ���� ���
	Force += GetRollingResistance();

	FVector Acceleration = Force / Mass;	// a = F/m (F = ��, m = ����)
	Velocity += Acceleration * DeltaTime;	// v = u + at (u = ó�� �ӵ�, a = ���ӵ�, t = �ð�)		

	ApplyRotation(DeltaTime);
	UpdateLocationFromVelocity(DeltaTime);	//�ӵ��� ���� ��ġ ������Ʈ
}

/// <summary>
/// ���� ���� ���
/// </summary>
/// <returns></returns>
FVector AGoKart::GetAirResistance()
{
	return -Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient;	// F = -v^2 * DragCoefficient
}

FVector AGoKart::GetRollingResistance()
{
	float AccelerationDueToGravity = -GetWorld()->GetGravityZ() / 100;	//�߷� ���ӵ� m/s^2�� ��ȯ
	float NormalForce = Mass * AccelerationDueToGravity;	// N = m * g
	return -Velocity.GetSafeNormal() * RollingResistanceCoefficient * NormalForce;	// F = -N * RollingResistanceCoefficient
}

void AGoKart::ApplyRotation(float DeltaTime)
{
	float DeltaLocation = FVector::DotProduct(GetActorForwardVector(), Velocity) * DeltaTime;	// v = s/t (v = �ӵ�, s = �Ÿ�, t = �ð�)
	float RotationAngle = DeltaLocation / MinTurningRadius * SteeringThrow;	// s = r * theta (r = ������, theta = ����)
	FQuat RotationDelta(GetActorUpVector(), RotationAngle);	//ȸ�� ���� ���

	Velocity = RotationDelta.RotateVector(Velocity);	//�ӵ� ���Ϳ� ȸ�� ����	
	AddActorWorldRotation(RotationDelta);	//ȸ�� ����
}

void AGoKart::UpdateLocationFromVelocity(float DeltaTime)
{
	FVector Translation = Velocity * 100 * DeltaTime;	// cm/s -> m/s ��ȯ

	FHitResult HitResult;
	AddActorWorldOffset(Translation, true, &HitResult);

	//�浹 ó��
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
	Server_MoveForward(Value.Get<float>());
}

void AGoKart::Steering(const FInputActionValue& Value)
{
	Server_MoveRight(Value.Get<float>());
}

/// <summary>
/// RPC �Լ�
/// </summary>
/// <param name="Value"></param>
void AGoKart::Server_MoveForward_Implementation(float Value)
{
	ThrottleValue = Value;
}

bool AGoKart::Server_MoveForward_Validate(float Value)
{
	return FMath::Abs(Value) <= 1;
}

void AGoKart::Server_MoveRight_Implementation(float Value)
{
	SteeringThrow = Value;
}

bool AGoKart::Server_MoveRight_Validate(float Value)
{
	return FMath::Abs(Value) <= 1;
}


