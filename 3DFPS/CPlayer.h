#pragma once
#include "CCharacter.h"
#include "CInput.h"

class CPlayer :
    public CCharacter
{

public:

    enum PlayerState {
        Idle,
        Dashing,
        Walking,
        Jumping,
		Sliding,
        Attacking
	};

    CPlayer();
    virtual ~CPlayer() override;
	void InitPlayer();
    virtual void Update() override;
	virtual void Draw(SCENE_DATA& sceneData) override;

	void SetFloorY(float y) { m_FloorY = y; }
	void SetCeilingY(float y) { m_CeilingY = y; }
	void SetVelocity(const D3DXVECTOR3& vel) { m_Velocity = vel; }
	int SetCurrentWeapon(int weaponIndex) { return m_currWeapon = weaponIndex; }

	void SetShooting(bool value) { m_CanShoot = value; }
	void SetDash(bool value) { m_CanDash = value; }
	void SetJump(bool value) { m_CanJump = value; }
	void SetCrouch(bool value) { m_CanCrouch = value; }
	void SetGravity(bool value) { m_IsGravityEnabled = value; }
	void SetInertia(bool value) { m_IsInertiaEnabled = value; }

	bool IsAlive() const { return m_Health > 0.0f; }
	bool CanShoot() const { return m_CanShoot; }
	bool IsJumping() const { return m_IsJumping; }
	bool IsDashing() const { return m_IsDashing; }
	bool IsGrounded() const { return m_IsOnGround; };	//デバッグ用


	D3DXVECTOR3 GetForwardVector() const { return m_Forward; }
	D3DXVECTOR3 GetRightVector() const { return m_Right; }
	D3DXVECTOR3 GetVelocity() const { return m_Velocity; }
	D3DXVECTOR3 GetAcceleration() const { return m_Acceleration; }

	int GetCurrentWeapon() const { return m_currWeapon; }
	int GetState() const { return m_State; }

	float GetHealth() const { return m_Health; }
	float GetMoveSpeed() const { return m_MoveSpeed; }
	float GetJumpStrength() const { return m_JumpStrength; }
	float GetHeight() const { return m_Height; }
	float GetDashTimer() const { return m_DashTimer; }

	CROSSRAY GetHeadCrossRay() const { return *m_pHeadCrossRay; }

	void ApplyDamage(float damage);
	void ApplyHeal(float heal) { m_Health += heal;  }
	void ApplyStamina(float stamina) { m_DashTimer += stamina; }
	void ApplyAmmo(int ammo) { return; };

	int NextWeapon() { return m_currWeapon = (m_currWeapon + 1) % 3; }	// 0,1,2
	int PrevWeapon() 
	{
		return m_currWeapon = 
			((m_currWeapon - 1) < 0 ? 
				(4 + (m_currWeapon - 1) * -1) : 
				(m_currWeapon - 1)) % 3; 
	}

	void UpdateAxis();

	void UpdateCrossRay();

private:
	
	void HandleInput();
	void HandleAirPhys();
	void CalculateVectors();
	void CalculateInertia();
	void ApplyForce(const D3DXVECTOR3& force, float mass);
	
	void Move();
    void Shoot();
	void Jump();
	void Dash();
	void Slide();
	void SpecialAction();


private:
	
	D3DXVECTOR3 m_Velocity;
	D3DXVECTOR3 m_Inertia;
	D3DXVECTOR3 m_Acceleration;
	D3DXVECTOR3 m_Forward;
	D3DXVECTOR3 m_Right;
	D3DXVECTOR3 m_DashDirection;

	PlayerState m_State;
    float m_MoveSpeed;
	float m_JumpStrength;
	float m_Health;
	float m_Height;
	int m_currWeapon;
	CInput* m_pInputHandler;

	float m_ShootCooldownTimer;
	bool m_CanShoot = true;
	float m_DashTimer;

	bool m_CanDash = true;
	bool m_CanJump = true;
	bool m_CanCrouch = true;
	bool m_CanSlide = true;
	bool m_CanMove = true;
	
	bool m_IsGravityEnabled = true;
	bool m_IsInertiaEnabled = true;
	
	bool m_IsOnGround = true;
	bool m_IsJumping = false;
	bool m_IsDashing = false;
	bool m_IsCrouching = false;
	bool m_IsSliding = false;

	bool m_InvFrame = false;
	float m_InvFrameTimer = 0.0f;
	
	float m_FloorY = 0.f;
	float m_CeilingY = FLT_MAX;

	CROSSRAY* m_pHeadCrossRay;
};

