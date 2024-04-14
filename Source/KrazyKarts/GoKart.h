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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* SteeringAction;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	FVector GetAirResistance();
	FVector GetRollingResistance();

	void ApplyRotation(float DeltaTime);
	void UpdateLocationFromVelocity(float DeltaTime);

	/** Handles throttle input */
	void Throttle(const struct FInputActionValue& Value);
	void Steering(const struct FInputActionValue& Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveForward(float Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveRight(float Value);
	

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

	/// <summary>
	/// ������ Ʋ���� ���¿��� ȸ�� �ݰ��� �ּ� ������ (m)
	/// </summary>
	UPROPERTY(EditAnywhere)
	float MinTurningRadius = 10;

	UPROPERTY(EditAnywhere)
	float DragCoefficient = 16;

	/// <summary>
	/// ���� ����: ���ϼ��� ������ ŭ
	/// </summary>
	UPROPERTY(EditAnywhere)
	float RollingResistanceCoefficient = 0.015;

	FVector Velocity;

	float ThrottleValue;
	float SteeringThrow;
};
