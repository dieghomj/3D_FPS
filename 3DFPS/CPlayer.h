#pragma once
#include "CStaticMeshObject.h"
#include "CAnimCharacter.h"
#include "CInput.h"


class CPlayer 
	: public CAnimCharacter
{



public:

	const float MAX_LIGHT_INT = 3.0f;	//懐中電灯の最大明るさ

	enum MoveState {
		Forward,
		Left,
		Right,
		Backward,
		Up,
		Down,
		Stop
	};

	enum PlayerState {
		Idle = 0,
		Attacking = 1,
		Running = 3,
		Jumping = 2,
		Damaged = 4,
	};

	//アニメーション状態
	//プレイヤーモデルによる修正が必要
	enum AnimationState {
		AnimDamaged = 0 ,
		AnimIdle = 3,
		AnimAttack = 1,
		AnimJump = 4,
		AnimRun = 2,
	};

public:
	CPlayer();
	virtual ~CPlayer() override;
	virtual void Update() override;
	virtual void Draw(SCENE_DATA& sceneData) override;

	bool IsFlashOn() const {
		return m_bIsFlashOn;
	}

	void SetTankControlMode(bool mode) {
		m_bTankControlMode = mode;
	}
	void SetPlayerState(PlayerState state) {
		m_PlayerState = state;
	}
	int GetPlayerState() const {
		return m_PlayerState;
	}

	float GetPlayerHealth() const {
		return m_PlayerHealth;
	}

	float GetLightIntensity() const {
		return m_LightIntensity;
	}

	D3DXVECTOR3 GetVelocity() const {
		return m_vVelocity;
	}

	D3DXVECTOR3 GetDirection() const {
		return m_vDirection;
	}

	void ApplyDamage(float damage);
	void ApplyHeal(float heal);
	void ApplyLightEffect(float amount);

private:
	void AnimControl();
	void RadioControl();
	void HandleInput();

protected:

	D3DXVECTOR3		m_vVelocity;				//速度ベクトル
	D3DXVECTOR3		m_vDirection;				//移動方向ベクトル
	float			m_PlayerHealth;				//プレイヤーの体力
	float			m_LightIntensity;			//懐中電灯の明るさ

	float			m_TurnSpeed;				//回転速度
	float			m_MoveSpeed;				//移動速度
	MoveState		m_MoveState;				//移動状態
	PlayerState		m_PlayerState;				//プレイヤー状態
	AnimationState	m_AnimationState;			//アニメーション状態
	CInput*			m_pInput;					//入力管理クラス
	bool			m_bTankControlMode = false;	//タンク操作モードかどうか
	bool			m_bIsFlashOn = false;		//懐中電灯が点いているかどうか

};

