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

	FVector Acceleration = Force / Mass;	// a = F/m (F = ��, m = ����)
	Velocity += Acceleration * DeltaTime;	// v = u + at (u = ó�� �ӵ�, a = ���ӵ�, t = �ð�)

	ApplyRotation(DeltaTime);
		

	UpdateLocationFromVelocity(DeltaTime);	//�ӵ��� ���� ��ġ ������Ʈ
}

void AGoKart::ApplyRotation(float DeltaTime)
{
	float RotationAngle = MaxDegreesPerSecond * SteeringThrow * DeltaTime;	//ȸ���� ���
	FQuat RotationDelta(GetActorUpVector(), FMath::DegreesToRadians(RotationAngle));	//ȸ���� ���

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
	ThrottleValue = Value.Get<float>();
}

void AGoKart::Steering(const FInputActionValue& Value)
{
	SteeringThrow = Value.Get<float>();
}


