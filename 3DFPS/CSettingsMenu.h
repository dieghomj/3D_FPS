#pragma once

// 前方宣言.
class CFont;

//********************************************************************************
//	設定メニューUIクラス.
//		音量とマウス感度を調整する共通UI部品.
//		タイトル画面のメニューとゲーム中のポーズメニューの両方から再利用する.
//		キーボード操作（上下で項目選択／左右で値変更）に対応する.
//********************************************************************************
class CSettingsMenu
{
public:
	CSettingsMenu();

	// 選択状態を先頭の項目へ初期化する.
	void Reset();

	// 入力を処理し設定値を更新する.
	// 「戻る」が選択された、またはESCが押された場合は true を返す.
	bool Update();

	// 設定メニューを描画する（baseY は項目の描画開始Y座標）.
	void Draw(CFont* pFont, float baseY);

private:
	// メニュー項目.
	enum ROW
	{
		ROW_VOLUME = 0,	// 音量.
		ROW_SENSE,		// マウス感度.
		ROW_BACK,		// 戻る.
		ROW_COUNT
	};

	// 値スライダーのバー文字列を生成する（filledRatio は 0.0～1.0）.
	void BuildBar(float filledRatio, TCHAR* outBuffer, int bufferCount) const;

	int m_selected;	// 現在選択中の項目.
};
