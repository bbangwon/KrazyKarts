// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKart.generated.h"

USTRUCT()
struct FGoKartMove
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	float ThrottleValue;

	UPROPERTY()
	float SteeringThrow;

	UPROPERTY()
	float DeltaTime;

	UPROPERTY()
	float Time;
};

USTRUCT()
struct FGoKartState
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	FVector Velocity;

	UPROPERTY()
	FGoKartMove LastMove;
};


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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	FVector GetAirResistance();
	FVector GetRollingResistance();

	void ApplyRotation(float DeltaTime);
	void UpdateLocationFromVelocity(float DeltaTime);

	/** Handles throttle input */
	void Throttle(const struct FInputActionValue& Value);
	void Steering(const struct FInputActionValue& Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FGoKartMove Move);

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

	/// <summary>
	/// ����
	/// </summary>	
	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FGoKartState ServerState;

	FVector Velocity;
	
	UFUNCTION()
	void OnRep_ServerState();

	UPROPERTY(Replicated)
	float ThrottleValue;

	UPROPERTY(Replicated)
	float SteeringThrow;
};
