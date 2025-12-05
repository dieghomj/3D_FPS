#pragma once

#include "CGameObject.h"

#include "CSprite2D.h"

/************************************************************
*	UIオブジェクトクラス.
**/
class CUIObject
	: public CGameObject
{
public:
	CUIObject();
	virtual ~CUIObject() override;

	//CGameObjectで純粋仮想関数の宣言がされてるのでこちらで定義を書く.
	virtual void Update() override;
	//CUIObjectで宣言した関数で、以降はこれをoverrideさせる
	virtual void Draw();

	//スプライトを接続する.
	void AttachSprite( CSprite2D& pSprite ){
		m_pSprite = &pSprite;
	}
	//スプライトを切り離す.
	void DetachSprite(){
		m_pSprite = nullptr;
	}
	//パターン番号を設定
	void SetPatternNo(SHORT x, SHORT y){
		m_PatternNo.x = x;
		m_PatternNo.y = y;
	}

	void SetFillPercent(float percent, bool horizontal) {
		if (m_pSprite) {
			m_pSprite->SetFillPercent(percent, horizontal);
		}
	}

	void SetAlpha(float alpha) {
		if (m_pSprite) {
			m_pSprite->SetAlpha(alpha);
		}
	}

	POINTS GetPatternNo() const {
		return m_PatternNo;
	}

protected:
	//CGameObjectから継承した関数
	//final : これ以降はoverrideさせない
	void Draw(SCENE_DATA& sceneData) override final;

protected:
	CSprite2D*	m_pSprite;
	POINTS		m_PatternNo;	//パターン番号(マス目)
};
