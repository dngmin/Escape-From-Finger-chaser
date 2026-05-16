# Escape From Finger Chaser
- カメラを用いたチェイスゲーム（Chase Game）

## Demo
- **製作中**

## 技術スタック
- **作成中**

## 特徴
- **作成中**
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
 - **作成中**

## インストール方法 & 使い方
- **作成中**
### 0. ダウンロード (Download)
    - ダウンロード方法は以下の二つである。
#### **Method A: Git Clone（おすすめ）**
```bash
git clone https://github.com/dngmin/Escape-From-Finger-chaser.git
```
- ダウンロード先を指定したい場合は以下を先に行う

#### Method B: ZIPファイルダウンロード
1. [Github レポジトリ](https://github.com/dngmin/Escape-From-Finger-chaser)右上の緑[<> Code]ボタンをクリックする。
2. [Downlaod ZIP]を選択し、圧縮ファイルをダウンロード
3.解答の後、フォルダへ移動
#### **⚠️ A,Bどれもclone,解凍の後にはフォルダに移動が必要**
```bash
cd Path/Escape-From-Finger-chaser # e.g. cd desktop/user/Escape-From-Finger-chaser
```

### 1. 環境変数設定(.env)
- プロジェクトディレクトリに.envファイルを生成以下のパラメータを指定します。
    1. host_number
    2. port_number
- 書き方の例
```
port_number = 5005
host_number = 127.0.0.1
```

### CMakeの設定, ライブラリインストール、その他

## Road map
- mainコードからfunction, class, structのファイル分け
- 敵の動き追加
    - [x] 単純追跡
    - [x] オイラー法
    - [ ] ランダム動き
    - [ ] 未定