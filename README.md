# **Escape From Finger Chaser**
- カメラを用いたチェイスゲーム（Chase Game）

## Demo
![demo](./assets/demo.gif)

## 技術スタック
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
    - `opencv-python` (4.13.0.92) : カメラ映像入及びイメージ処理
- **C++**
    - `SFML` (Simple and Fast Multimedia Library) : グラフィックUI及びSocket通信

## 特徴
- **手の揺れ(jitter)補正** : 1€ Filterをを用いてランダムな動きでも補正を行う
- **4種の追跡アルゴリズム** : ユーザーを追跡するため異なる追跡アルゴリズムを複数使用
    - **単純追跡**
    - **オイラー法**
    - **ランダムウォーク**
    - **未定**
- **cross language通信** : UDPを用いた Python - C++間の分担
    - **Python** : mediapipe, opencv等の画像処理・認識
    - **C++** : 追跡ロジックアルゴリズム、UI

## Architecture & Program Flow
### Architecture
- Input : Mediapipeで検出した生の座標データ(`received_pos`)が入ります。x, y座標をfloat形式にパッキングしUDP通信でC++ロジック側に渡す。
- Processing
    - 渡されたデータの時間間隔(`dT`)を測るタイマー、速度を計算する数式、速度から精密にノイズを削る1€ フィルター(One Euro Filter)が結合されている。
    - 補正された現在座標に基づき、オイラー法(Euler method)を用いてnステップ後の位置を予測する。
- Output
    - ユーザーやChaserの位置座標に基づき描画。
    - 補正された現在座標(`get_curr_pos`)とnステップ後の位置座標(`get_Euler_predict`)にChaserが向かい、Chaserの位置を更新

### Program Flow
0. .envから環境変数取得
- ここからはループ
1. Mediapipeから人差し指先の座標を取得
2. float形式にパッキングしUDP送信
3. ユーザーによるプログラム終了確認→終了
4. UIウィンドウサイズ変化確認　→　視点変更
5. UDP受信(座標データ)確認　→　1Eurofilterノイズ補正
6. 補正及び予測座標に基づきChaser情報更新
7. 描画(Rendering)
8. オブジェクト間当たり判定
- 繰り返し
9. 終了

## インストール方法 & 使い方
- ⚠️MacOS開発環境であるため、他のOSについてはTest未実施
    - 以下もMacOSを基準に説明している
 - **Python3.10以上**
 - **C++20以上**

### 0. ダウンロード (Download)
- ダウンロード方法は以下の二つである。
#### **Method A: Git Clone（おすすめ）**
```bash
git clone https://github.com/dngmin/Escape-From-Finger-chaser.git
```
- ダウンロード先を指定したい場合は以下を先に行う

#### Method B: ZIPファイルダウンロード
1. [Github レポジトリ](https://github.com/dngmin/Escape-From-Finger-chaser)右上の緑[<> Code]ボタンをクリックする。
2. [Download ZIP]を選択し、圧縮ファイルをダウンロード
3.解凍の後、フォルダへ移動
#### **⚠️ A,Bどれもclone,解凍の後にはフォルダに移動が必要**
```bash
cd Path/Escape-From-Finger-chaser # e.g. cd desktop/user/Escape-From-Finger-chaser
```

### 1. 環境変数設定(.env)
- プロジェクトディレクトリに.envファイルを生成以下のパラメータを指定します。
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
- 現在はそれぞれのスクリプトを手動で実行する必要がある(今後改善予定)
```bash
# 両方ともプロジェクトディレクトリから実行する必要がある。

# ターミナル 1: Python Hand tracking実行
python src/python/hand_tracking.py

# ターミナル 2: C++ UI実行
./build/effc
```

## Road map
- mainコードからfunction, class, structのファイル分け
    - [x] Python
    - [ ] C++
- 例外処理
    - [x] Python
    - [ ] C++
- 敵の動き追加
    - [x] 単純追跡
    - [x] オイラー法
    - [ ] ランダム動き
    - [ ] 未定
- python mediapipeのrunning_modeをLIVE_STREAMに変更
    - call backを用いてデータ送信をmain loopから外せる
- hand_tracking.pyとmain.cppを同時に実行するファイル作成
- edgeでの異常の動き発生対策
- 高速で手を動かすと生じるフレームドロップ及びテレポート対策
- chaserたちの位置が重なる時での対策
- ユーザーが止まっているとき生じる意図してないchaser騙し対策
- Program Flowをmermaidに変更