# Landmark検出及び画像処理
import cv2
import mediapipe as mp
from mediapipe.tasks.python import vision
import time

# 通信
import socket
import struct

# データ読み込み
import os

# カスタム例外処理
class CameraNotFoundError(Exception): pass
class envNotFoundError(Exception): pass

# mediapipe setting
class Mediapipe_Setting:
    def __init__ (self, model_path):
        # mediapipe hand landmarker model & setting
        base_options = mp.tasks.BaseOptions(model_asset_path = model_path)
        option = vision.HandLandmarkerOptions(
            base_options = base_options,
            num_hands = 1, # 最大１つの手間で認識
            running_mode = vision.RunningMode.VIDEO,
        )
        self.landmarker = vision.HandLandmarker.create_from_options(option)
        # index 8 = 人差し指
        self.index_finger_idx = 8
    
    def init_camera(self):
        default_camera = 0
        self.cap = cv2.VideoCapture(default_camera)
        if not self.cap.isOpened() : raise CameraNotFoundError("OpenCV: failed to connect camera")

        # 開始時刻を記録
        # timestampの計算の起点
        self.start_time = time.time()
    
    def main_loop(self, socket_setting):
        while self.cap.isOpened():
            ret, frame = self.cap.read()
            if not ret: break
            # timestamp更新
            # 単位がmsであるため1000をかける
            self.timestamp_ms = int((time.time() - self.start_time) * 1000)

            if self.convert_image(frame):
                self.detect_and_send(socket_setting)
        # データをreleaseして終了
        self.cap.release()
        cv2.destroyAllWindows()
    
    # 画像処理
    def convert_image(self,frame):
        try:
            # 左右反転
            frame = cv2.flip(frame, 1)
            # BGR配列からRGBに変換
            rgb = cv2.cvtColor(frame,cv2.COLOR_BGR2RGB)
            # Mediapipeが処理できるタイプに変換
            self.mp_image = mp.Image(image_format=mp.ImageFormat.SRGB, data = rgb)
            return True
        except:
            print("[Error] イメージ変換失敗。このフレーム処理はスキップします")
            return False

    # 画像から手を検出し、socket通信を行う
    def detect_and_send(self, socket_setting):
        # Mediapipe 処理
        result = self.landmarker.detect_for_video(self.mp_image, int(self.timestamp_ms))

        # 画像から手が検出された場合
        if result.hand_landmarks:
            for landmarks in result.hand_landmarks:
                cx, cy = landmarks[self.index_finger_idx].x, landmarks[self.index_finger_idx].y
            # floatに変換し送信
            data = struct.pack("2f",cx, cy)
            
            # sock.sendto(data, server_address)
            socket_setting.send(data)

# socket setting
class Socket_Setting:
    def __init__(self, host_number, port_number):
        # AF_INET = Address Family Internet
        # 最も使用されているIPv4式
        # SOCK_DGRAM = UDP
        # TCPと違ってデータ送信有無と関係ない速度重視手法
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.server_address = (host_number, port_number)
    
    def send(self, data):
        self.sock.sendto(data, self.server_address)

# read .env
def read_env():
    environ = dict()
    # .envファイルが存在したら、読み込む
    if not os.path.exists(".env"): raise envNotFoundError("failed to open .env file")
    if os.path.exists(".env"):
        with open(".env") as env:
            # .envファイルを1行づつ読み込む
            for line in env:
                # "="の入ってない間違えた書き方及びコメントであるか確認
                if "=" in line and not line.startswith("#"):
                    # 行を分解し、キーとバリューに分ける
                    key, value = line.strip().split("=")
                    #空白除去
                    key = key.strip()
                    value = value.strip()
                    environ[key] = value
    # port numberは整数に変換する必要がある
    environ["port_number"] = int(environ["port_number"])
    return environ

if __name__ == "__main__":
    try:
        # initial setting
        # read .env
        environ = read_env()
        # mediapipe 初期化
        mediapipe = Mediapipe_Setting(environ["model_path"])
        mediapipe.init_camera()
        # socket 初期化
        socket_setting = Socket_Setting(environ["host_number"], environ["port_number"])
        # main loop
        mediapipe.main_loop(socket_setting)

    except Exception as e:
        error_msg = repr(e)
        print(e)
        # エラーメッセージを赤で出力
        print("\033[31m", end="")
        # エラーメッセージ
        
        # Mediapipe & Opencv
        if "Unable to open file" in error_msg:
            print("[Error] Mediapipeモデルを開くことができません")
            print("→ 解決方法: .env内の'model_path'と実際モデルのパスが一致するか確認してください")
        elif "Unable to get file" in error_msg:
            print("[Error] Mediapipeモデルが壊れています")
            print("→ 解決方法: 新しくモデルをダウンロードしてください")
        elif "OpenCV: failed to connect camera" in error_msg:
            print("[Error] カメラデバイスに接続できません")
            print("→ 解決方法: カメラデバイスが接続されているか確認してください")
        elif "timestamp must be monotonically increasing" in error_msg:
            print("[Error] Mediapipe time stamp エラーで終了します")
            print("→ 解決方法: time stampは狭義の単調増加である必要があります。PCの時間基盤演算システムを確認してください")

        # read .env
        elif "failed to open .env file" in error_msg:
            print("[Error] .envファイルを開くことができません")
            print("→ 解決方法: .envファイルがあるか又はPathを確認してください")
        elif "por_number" in error_msg:
            print("[Error] port numberが見つかりません")
            print("→ 解決方法: .envファイルにport_numberがあるか確認してください")
        elif "host_number" in error_msg:
            print("[Error] host numberが見つかりません")
            print("→ 解決方法: .envファイルにhost_numberがあるか確認してください")
        elif "model_path" in error_msg:
            print("[Error] model pathが見つかりません")
            print("→ 解決方法: .envファイルにmodel_pathがあるか確認してください")
        elif "invalid literal for int" in error_msg:
            print("[Error] port numberは整数である必要があります")
            print("→ 解決方法: .envファイルのport_numerを確認してください")
        
        '''
        Exceptメッセージテンプレート
        print("\033[31m  \033[0m")
        elif "" in error_msg:
            print("[Error] ")
            print("→ 解決方法: ")
        '''

        # 色変換を終了
        print("\033[0m")




# pip install opencv-python mediapipe