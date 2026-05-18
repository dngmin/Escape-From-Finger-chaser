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
- **手の揺れ(jitter)補正** : 1€ Filterをを用いたランダムワークでも補正を行う
- **4種の追跡アルゴリズム** : ユーザーを追跡するため異なる追跡アルゴリズムを複数使用
    - **単純追跡**
    - **オイラー法**
    - **ランダムウォーク**
    - **未定**
- **cross language通信** : UDPを用いた Python - C++間の分担
    - **Python** : mediapipe, opencv等の画像処理・認識
    - **C++** : 追跡ロジックアルゴリズム、UI

## Architecture & Program Flow
 - (**現在、構成図を作成中**)
    - 基本的な流れとして、Python側でMediaPipeを用いてての座標を取得し、UDP通信を介してC++側に座標データを送信、C++側で追跡アルゴリズムの計算とSFMLによる描画(UI機能)を行っている。

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
    1. host_number
    2. port_number
    3. model_path
- 書き方の例
```
port_number = 5005
host_number = 127.0.0.1
model_path = models/hand_landmarker.task
```
- **⚠️"="の前後に必ずスペースを一つ空けること : 読み込めない原因となる**

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
    - [ ] Python
    - [ ] C++
- 敵の動き追加
    - [x] 単純追跡
    - [x] オイラー法
    - [ ] ランダム動き
    - [ ] 未定
- python mediapipeのrunning_modeをLIVE_STREAMに変更
    - call backを用いてデータ送信をmain loopから外せる
- hand_tracking.pyとmain.cppを同時に実行するファイル作成