// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKart.generated.h"

UCLASS()
class KRAZYKARTS_API AGoKart : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGoKart();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* ThrottleAction;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void UpdateLocationFromVelocity(float DeltaTime);

	/** Handles throttle input */
	void Throttle(const struct FInputActionValue& Value);

	

	/// <summary>
	/// �ڵ����� ����(kg)
	/// </summary>
	UPROPERTY(EditAnywhere)
	float Mass = 1000;		

	/// <summary>
	/// ����Ʋ�� ������ ���������� ���� �������� �� (N)
	/// </summary>
	UPROPERTY(EditAnywhere)
	float MaxDrivingForce = 10000;  


	FVector Velocity;

	float ThrottleValue;
};
