#pragma once

class CTime
{
public:
    CTime();
    ~CTime();

    bool Init(double fps); // fps は希望する「固定」更新レート(例: 60.0)

    // 固定更新用に累積された時間を計算する
    void Tick();

    // 固定ステップを実行すべき場合に true を返す.
    // メインループは false が返るまでこの関数を繰り返し呼び出す.
    bool FixedTick(double& fixedDeltaTime);

    // 描画用の標準的な可変時間の更新
    void VariableTick();

    void StartTimer();
    double StopTimer();
    double GetTimerElapsed() const { return m_timerElapsed; };
	double GetFramePerSec() const { return m_framePerSec; };
    // 外部から使用するためのゲッター
    float GetDeltaTime() const { return deltaTime; }
    double GetFixedDeltaTime() const { return m_fixedStepDuration; }
    double GetTotalTime() const { return m_totalTime; }


protected:
    float deltaTime;        // 可変時間(描画/アニメーション用)
    double m_totalTime;     // 経過時間の合計

    double m_timerElapsed;  // タイマー開始からの経過時間

    LARGE_INTEGER m_lastTime;   // 最後に記録したカウンタ値
    LARGE_INTEGER m_qpFreq;     // パフォーマンスカウンタの周波数

    // 固定時間ステップ用の変数
    double m_fixedStepDuration; // 1回の固定更新の目標時間(例: 16.666 ms)
    double m_timeAccumulator;   // 前回の固定更新からの累積時間

	int m_frameCount;      // FPS計算用のフレーム数
	double m_timerFPS;        // FPS計算用のタイマー
	double m_framePerSec;       // 実際の毎秒フレーム数
};