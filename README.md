# **Finger Trace : Algorithm LAB**
MediaPipeを用いたAIによる手の座標トラッキング（Python）と、C++による高頻度な物理計算およびレンダリングを、UDPソケット通信で同期させた、リアルタイム・システムです。

現在は視覚的な演出よりも、**「入力ジッターの補正」** や **「複数言語間マルチプロセス通信」**といった**システムのコア・アーキテクチャの構築**に重点を置いて開発しています。


## 📽️ Demo
![demo](./assets/demo.gif)

## 🛠️ 技術スタック
### 開発環境
- **OS**
    - macOS
- **Languages**
    - Python 3.12+
    - C++ 20+
- **Build Tool**
    - CMake 3.12

### ライブラリ & フレームワーク
- **Python**
    - `mediapipe` (0.10.35) : AIを用いたLIVE Hand Landmark Detection
    - `opencv-python` (4.13.0.92) : カメラ映像入力および画像処理
- **C++**
    - `SFML` (Simple and Fast Multimedia Library) : グラフィックおよびソケット通信

## 💫 特徴
### 1. リアルタイム・トラッキング精度の追求
- **適応型ノイズ除去**: 1€ Filterを用いて、カメラの解像度や光源に依存しない**揺れのない**滑らかなレンダリングを実現しました。
- **物理シミュレーション**: 追跡オブジェクトの挙動に、オイラー法を用いた「予測アルゴリズム」を導入。さらに、一つの軸に沿って移動する「軸整列移動アルゴリズム」、動的座標系を用いた「相対的な乱数アルゴリズム」を組み合わせることで、単調ではない非線形な挙動を生成し、システムとして高度な追跡ロジックを確立しました。

※各アルゴリズムの数式や詳細なロジックは [こちら（追跡アルゴリズム詳細）](./assets/chasing_algorithm.md) をご参照ください。

### 2. システム設計
- **ハイブリッド・アーキテクチャ**: 画像認識プロセス（Python）とロジック・レンダリング(C++)をUDPで分離し、開発効率を求めました。
- **データ駆動型設計:** 接続情報やパス設定を`.env`ファイルで外部化し、コードを修正することなく環境変数を変更できる柔軟性を確保しています。

### 3. UXの最適化
- **解像度可変対応**: ウィンドウのリサイズに応じて座標変換行列を再計算し、常に正しいアスペクト比でトラッキングデータを反映させる設計にしました。

## 💡 Why This Technology?
このプロジェクトは、認識精度の追求とリアルタイムな操作感の両立を目指し、以下のような技術選定を行いました。

- **Python & C++ のハイブリッド構成**
    - **Python (`Mediapipe`/`OpenCV`)**: AIモデルの推論や画像処理において、開発効率を優先しました。
    - **C++ `(SFML)`**: 画像処理以外の「物理計算」「制御ロジック」「高頻度レンダリング」において、Pythonでは回避できない速度低下を避けるため、実行性能に優れたC++を採用しました。
- **UDPによる通信**
    - リアルタイムな操作感において **過去のデータより最新の座標**が最優先であるため、通信の信頼性よりも到達速度を優先するUDPソケット通信を採用しました。
- **1€ Filterの自作実装**
    - 固定係数フィルタリングでは、手の動きが速い時の**遅延**と、止まっている時の **ジッター(揺れ)** を同時に解消できません。速度に応じてフィルタ強度が動的に変化する1€ Filterを実装することで、滑らかな操作体験を実現しました。

## 🏗️🔄 Architecture & Program Flow
### 🏗️ Architecture
- Input : Mediapipeで検出した生の座標データ(`received_pos`)が入ります。x, y座標をfloat形式にパッキングしUDP通信でC++ロジック側に渡します。
- Processing
    - 渡されたデータの時間間隔(`dT`)を測るタイマー、速度を計算する数式、速度から精密にノイズを削る1€ Filter(One Euro Filter)が結合されています。
    - 補正された現在座標に基づき、オイラー法(Euler method)を用いてnステップ後の位置を予測します。
    - 追跡オブジェクトの現在位置を基準（原点）とした動的座標系を定義し、ターゲットが存在する象限エリアに限定した乱数座標を生成します。
    - 外部入力（操作対象）の速度ベクトルの主軸（移動量の大きい軸）を特定し、その軸を維持した座標を生成します。
- Output
    - 各オブジェクトの位置座標に基づきレンダリング。
    - 補正された現在座標（`player_curr_pos`）、nステップ後の予測座標（`player_Eulerpredict_pos`）、軸整列座標（`AxisAligned_pos`）および生成された乱数座標（`random_pos`）の3つをターゲットとしてChaserの位置を更新します。

### 🔄 Program Flow
0. .envから環境変数取得
- ここからはループ
1. Mediapipeから人差し指先の座標を取得
2. float形式にパッキングしUDP送信
3. ユーザーによるプログラム終了確認→終了
4. UIウィンドウサイズ変化確認　→　視点変更
5. UDP受信(座標データ)確認　→　1€ Filterノイズ補正
6. 補正及び予測座標に基づき追跡オブジェクト情報更新
7. 描画(Rendering)
8. オブジェクト間衝突判定
- 繰り返し
9. 終了

> [プログラムの流れをフローチャートで見る](./assets/flowchart.md)

## 🧳 インストール方法 & 使い方
- ⚠️MacOS開発環境であるため、他のOSについてはTest未実施
    - 以下もMacOSを基準に説明しています。
 - **Python3.10以上**
 - **C++20以上**

### 0. ダウンロード (Download)
- ダウンロード方法は以下の2つです。
#### **Method A: Git Clone（おすすめ）**
```bash
git clone https://github.com/dngmin/FingerTrace-AlgorithmLAB.git
```
- ダウンロード先を指定したい場合は以下を先に行なってください。

#### Method B: ZIPファイルダウンロード
1. [Github リポジトリ](https://github.com/dngmin/FingerTrace-AlgorithmLAB.git)右上の緑[<> Code]ボタンをクリックしてください。
2. [Download ZIP]を選択し、圧縮ファイルをダウンロードしてください。
3.解凍の後、プロジェクトフォルダへ移動してください。
#### **⚠️ A, Bどちらの方法でも、Clone・解凍後はフォルダへの移動が必要です**
```bash
cd Path/FingerTrace-AlgorithmLAB # e.g. cd desktop/user/FingerTrace-AlgorithmLAB
```

### 1. 環境変数設定(.env)
- プロジェクトディレクトリに **.env** ファイルを生成し、以下のパラメータを指定してください。
    - host_number
    - port_number
    - model_path
- 書き方の例
```
host_number = 127.0.0.1
port_number = 5005
model_path = models/hand_landmarker.task
```

### 2. ライブラリ設定
- Python
```bash
pip install -r "requirements.txt"
#もしくは
python -m pip install -r "requirements.txt"
```
- C++
```bash
brew install sfml
```

### 3. CMake
```bash
# 1. ビルド用ディレクトリの作成と移動
mkdir build && cd build

# 2. CMakeの実行(Makefileの生成)
cmake ..

# 3. コンパイルの実行
make
```

### 4. 実行
- 現在はそれぞれのスクリプトを手動で実行する必要があります。(今後改善予定)
```bash
# 両方ともプロジェクトディレクトリから実行する必要がある。

# ターミナル 1: Python Hand tracking実行
python src/python/hand_tracking.py

# ターミナル 2: C++ UI実行
./build/ftal
```

## 🏁 Current Challenges & Future Improvements
- 進行中
    - プログラム開始と同時にスタート、追跡オブジェクトに当たると即終了によるUX低下改善
        - [ ]初期UI画面
        - [ ]終了エフェクト及び終了画面
- 予定（※ 優先度順ではありません）
    - python mediapipeのrunning_modeをLIVE_STREAMに変更
        - call backを用いてデータ送信をmain loopから外せる
    - hand_tracking.pyとmain.cppを同時に実行するファイル作成
    - edgeでの異常の動き発生対策
    - 高速で手を動かすと生じるフレームドロップ及びテレポート対策
    - 追跡オブジェクト同士が重なる際での対策
    - ビジュアルアップグレード(背景、プレイヤー、追跡オブジェクトなど)
    - C++ヘッダーファイルで前方宣言を使用検討
    - **1ファイルの修正による他ファイルへの影響が大きいと感じたため、依存関係を解消するリファクタリングの必要性を実感**