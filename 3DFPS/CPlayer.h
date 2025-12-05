#pragma once
#include "CCharacter.h"
#include "CInput.h"

class CPlayer :
    public CCharacter
{

public:

	const float WALK_SPEED = 0.2f;
	const float DASH_SPEED = 0.5f;
	const float DASH_DISTANCE = 1.5f;
	const float JUMP_STRENGTH = 0.18f;
	const float GRAVITY = 0.0098f;
	const float FRICTION = 0.090f;
	const float SHOOT_COOLDOWN = 0.5f; // seconds
	const float DASH_DURATION = 0.03f; // seconds
	const float HEALTH_MAX = 100.0f;
	const float PLAYERSIZE = 2.0f;

    enum PlayerState {
        Idle,
        Dashing,
        Walking,
        Jumping,
        Attacking
	};

    CPlayer();
    virtual ~CPlayer() override;
    virtual void Update() override;
	virtual void Draw(SCENE_DATA& sceneData) override;

	void SetFloorY(float y) { m_FloorY = y; }

	bool IsAlive() const { return m_Health > 0.0f; }
	bool CanShoot() const { return m_CanShoot; }
	bool IsJumping() const { return m_IsJumping; }
	bool IsDashing() const { return m_IsDashing; }

	D3DXVECTOR3 GetForwardVector() const { return m_Forward; }
	D3DXVECTOR3 GetRightVector() const { return m_Right; }
	D3DXVECTOR3 GetVelocity() const { return m_Velocity; }
	D3DXVECTOR3 GetAcceleration() const { return m_Acceleration; }

	int GetCurrentWeapon() const { return m_currWeapon; }
	int GetState() const { return m_State; }

	int SetCurrentWeapon(int weaponIndex) { return m_currWeapon = weaponIndex; }
	int NextWeapon() { return m_currWeapon = (m_currWeapon + 1) % 3; } // Assuming 3 weapons

private:
	
	void HandleInput();
	void CalculateVectors();
	void CalculateInertia();
	void ApplyForce(const D3DXVECTOR3& force);
	
	void Move();


    void Shoot();
	void Jump();
	void Dash();



	bool IsGrounded() const { return m_IsOnGround; };

private:
	
	D3DXVECTOR3 m_Velocity;
	D3DXVECTOR3 m_Inertia;
	D3DXVECTOR3 m_Acceleration;
	D3DXVECTOR3 m_Forward;
	D3DXVECTOR3 m_Right;

	PlayerState m_State;
    float m_MoveSpeed;
	float m_JumpStrength;
	float m_Health;
	int m_currWeapon;
	CInput* m_pInputHandler;

	float m_ShootCooldownTimer = 0.f;
	float m_DashTimer = 0.f;

	bool m_IsOnGround = true;
	bool m_CanShoot = true;
	bool m_IsJumping = false;
	bool m_IsDashing = false;
	
	float m_FloorY = 0.f;

};

