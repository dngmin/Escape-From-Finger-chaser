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

# read .env
environ = dict()
if os.path.exists(".env"):
    with open(".env") as env:
        for line in env:
            if "=" in line and not line.startswith("#"):
                key, value = line.strip().split(" = ")
                environ[key] = value
environ["port_number"] = int(environ["port_number"])

# mediapipe hand landmarker model & setting
model_path = "models/hand_landmarker.task"

base_options = mp.tasks.BaseOptions(model_asset_path = model_path)
option = vision.HandLandmarkerOptions(
    base_options = base_options,
    num_hands = 1, # 最大1つの手まで認識
    running_mode = vision.RunningMode.VIDEO,
    )
landmarker = vision.HandLandmarker.create_from_options(option)

# socket setting
# SOCK_DGRAM = UDP
# TCPと違ってデータ送信有無と関係なく速度重視
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_address = (environ["host_number"], environ["port_number"])


cap = cv2.VideoCapture(0)
# カメラデバイスの情報
# 解像度(縦横の長さ)
width = cap.get(cv2.CAP_PROP_FRAME_WIDTH)
height = cap.get(cv2.CAP_PROP_FRAME_HEIGHT)
# fps
fps = cap.get(cv2.CAP_PROP_FPS)
if fps == 0: fps = 30.0 # fpsデータがうまく得られない場合

timestamp_ms = 0.0
dt = 1000.0 / fps # msが基準対位のため 1000をかける

while cap.isOpened():
    ret, frame = cap.read()
    if not ret: break
    # timestamp更新
    timestamp_ms += dt

    # 画像処理
    # 左右反転
    frame = cv2.flip(frame, 1)
    # モノクロ化
    rgb = cv2.cvtColor(frame,cv2.COLOR_BGR2RGB)
    # Mediapipeが処理できるタイプに変換
    mp_imgae = mp.Image(image_format=mp.ImageFormat.SRGB, data = rgb)
    
    # Mediapipe 処理
    result = landmarker.detect_for_video(mp_imgae, int(timestamp_ms))

    # 画像から手が検出された場合
    if result.hand_landmarks:
        for landmarks in result.hand_landmarks:
            # index 8 = 人差し指
            cx, cy = landmarks[8].x, landmarks[8].y
        # floatに変換し送信
        data = struct.pack("2f",cx, cy)
        sock.sendto(data, server_address)


cap.release()
cv2.destroyAllWindows()











# pip install opencv-python mediapipe