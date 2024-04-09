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

	FVector Translation = Velocity * 100 * DeltaTime;	// cm/s -> m/s ��ȯ

	AddActorWorldOffset(Translation, true);

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
	}
}

void AGoKart::Throttle(const FInputActionValue& Value)
{
	ThrottleValue = Value.Get<float>();
}


