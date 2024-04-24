// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GoKartMovementComponent.generated.h"

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

	bool IsValid() const
	{
		return FMath::Abs(ThrottleValue) <= 1 && FMath::Abs(SteeringThrow) <= 1;
	}
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAZYKARTS_API UGoKartMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGoKartMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SimulateMove(const FGoKartMove& Move);

	FVector GetVelocity() { return Velocity; }
	void SetVelocity(FVector Val) { Velocity = Val; }

	void SetThrottle(float Val) { ThrottleValue = Val; }
	void SetSteeringThrow(float Val) { SteeringThrow = Val; }

	FGoKartMove GetLastMove() { return LastMove; }

private:
	FGoKartMove CreateMove(float DeltaTime);

	FVector GetAirResistance();
	FVector GetRollingResistance();

	void ApplyRotation(float DeltaTime, float SteeringThrow);
	void UpdateLocationFromVelocity(float DeltaTime);

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

	FGoKartMove LastMove;
};
