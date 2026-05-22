# FAST ATTACK / 3D_FPS

A retro-style first-person shooter about speed, ammo, and stamina.

This game is built for players who love classic FPS games and for anyone who wants a quick, casual action run.  
The goal is simple: clear each stage as fast as possible while managing limited resources.

## English

### Game Overview
- Fast-paced FPS action with a retro feel
- Manage ammo, stamina, health, and time
- Clear stages quickly to unlock the next ones
- Score is based on kills, combo, remaining time, and deaths

### Code Base Overview
- `CMain` creates the window, DirectX devices, and scene flow
- `CSceneManager` switches between `Menu`, `Game`, `Result`, and `GameOver`
- `CGame` contains the core gameplay loop, enemies, stages, UI, effects, and scoring
- `CPlayer` handles movement, shooting, jumping, dashing, and crouch/slide behavior
- `CLevelController`, `CLevel`, and `CStage` manage stage progression and level layouts
- `CGameStats` stores run results, difficulty, unlocks, and score data

### Build
This project is a Visual Studio C++ solution.

Requirements:
- Visual Studio 2022
- Windows 10 SDK 10.0.19041.0 or later
- Legacy DirectX SDK (June 2010) path set through `DXSDK_DIR`
- The bundled `Effekseer` headers and libraries

Set `DXSDK_DIR` in your Windows environment variables or in the Visual Studio project settings so it points to the DirectX SDK install folder.

Steps:
1. Open `3DFPS.sln` in Visual Studio.
2. Select `x64` and `Debug` or `Release`.
3. Make sure `DXSDK_DIR` points to your DirectX SDK install.
4. Build the `3DFPS` project.

### Run
1. Build the solution.
2. Run the generated executable from the output folder.
3. Keep the `Data` folder next to the executable so textures, sounds, and effects load correctly.

### Controls
- Move: `W`, `A`, `S`, `D`
- Look around: Mouse
- Shoot: Left mouse button
- Jump: `Space`
- Dash: `Shift`
- Crouch / Slide: `Ctrl` or `C`
- Switch weapon: `Q` / `E`
- Menu navigation: Mouse click, arrow keys, `Enter`, `Esc`

## 日本語

### ゲーム概要
- レトロな雰囲気の高速FPSです
- 弾薬、スタミナ、体力、制限時間を管理します
- とにかく早くステージをクリアすることが目的です
- キル数、コンボ、残り時間、死亡回数でスコアを計算します

### コードベースの説明
- `CMain` がウィンドウ作成、DirectXの初期化、シーン遷移を担当します
- `CSceneManager` が `Menu`、`Game`、`Result`、`GameOver` を切り替えます
- `CGame` が敵、ステージ、UI、演出、スコアなどの本体をまとめています
- `CPlayer` が移動、射撃、ジャンプ、ダッシュ、しゃがみ／スライドを扱います
- `CLevelController`、`CLevel`、`CStage` がステージ進行と配置を管理します
- `CGameStats` が結果、難易度、解放状況、スコアを保持します

### ビルド方法
Visual Studio の C++ ソリューションです。

必要なもの:
- Visual Studio 2022
- Windows 10 SDK
- `DXSDK_DIR` で設定した DirectX SDK
- 同梱の `Effekseer` ヘッダとライブラリ

手順:
1. `3DFPS.sln` を Visual Studio で開きます。
2. `x64` と `Debug` または `Release` を選びます。
3. `DXSDK_DIR` が DirectX SDK の場所を指していることを確認します。
4. `3DFPS` プロジェクトをビルドします。

### 実行方法
1. ソリューションをビルドします。
2. 出力フォルダに生成された実行ファイルを起動します。
3. `Data` フォルダは実行ファイルと同じ場所に置いてください。テクスチャ、サウンド、エフェクトの読み込みに必要です。

### 操作方法
- 移動: `W` / `A` / `S` / `D`
- 視点移動: マウス
- 射撃: 左クリック
- ジャンプ: `Space`
- ダッシュ: `Shift`
- しゃがみ / スライド: `Ctrl` または `C`
- 武器切り替え: `Q` / `E`
- メニュー操作: マウスクリック、方向キー、`Enter`、`Esc`
