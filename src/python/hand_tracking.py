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
    
    def init_camera(self):
        # 0 = PCの default カメラデバイス
        self.cap = cv2.VideoCapture(0)
        # カメラデバイスのfps取得
        fps = self.cap.get(cv2.CAP_PROP_FPS)
        if fps == 0: fps = 30.0 # カメラデバイスは認識されたが(cv2.VideoCapture(0)を通過したから)、fpsデータがうまく得られない場合任意のfps30を与える

        # mediapipe時間経過確認用
        self.timestamp_ms = 0.0
        self.dt = 1000.0 / fps # msが基準対位のため 1000をかける
    
    def main_loop(self, socket_setting):
        while self.cap.isOpened():
            ret, frame = self.cap.read()
            if not ret: break
            # timestamp更新
            self.timestamp_ms += self.dt

            self.convert_image(frame)

            self.detect_and_send(socket_setting)
        # データをreleaseして終了
        self.cap.release()
        cv2.destroyAllWindows()
    
    # 画像処理
    def convert_image(self,frame):
        # 左右反転
        frame = cv2.flip(frame, 1)
        # BGR配列からRGBに変換
        rgb = cv2.cvtColor(frame,cv2.COLOR_BGR2RGB)
        # Mediapipeが処理できるタイプに変換
        self.mp_imgae = mp.Image(image_format=mp.ImageFormat.SRGB, data = rgb)

    # 画像から手を検出し、socket通信を行う
    def detect_and_send(self, socket_setting):
        # Mediapipe 処理
        result = self.landmarker.detect_for_video(self.mp_imgae, int(self.timestamp_ms))

        # 画像から手が検出された場合
        if result.hand_landmarks:
            for landmarks in result.hand_landmarks:
                # index 8 = 人差し指
                cx, cy = landmarks[8].x, landmarks[8].y
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
    if os.path.exists(".env"):
        with open(".env") as env:
            # .envファイルを1行づつ読み込む
            for line in env:
                # "="の入ってない間違えた書き方及びコメントであるか確認
                if "=" in line and not line.startswith("#"):
                    # 行を分解し、キーとバリューに分ける
                    # 指定した書き方("="の前後をスペースを一つ空ける)であることが前提
                    key, value = line.strip().split(" = ")
                    environ[key] = value
    # port numberは整数に変換する必要がある
    environ["port_number"] = int(environ["port_number"])
    return environ

if __name__ == "__main__":
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







# pip install opencv-python mediapipe