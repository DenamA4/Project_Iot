import json
# import websocket
import joblib
import pandas as pd

# Tải mô hình đã huấn luyện
model = joblib.load(r"D:\AI_NCKH\collision_model.pkl")

# Hàm xử lý dữ liệu nhận được qua WebSocket
def on_message(ws, message):
    try:
        # Tách dữ liệu 'a,b' thành hai giá trị
        data = message.split(',')
        
        if len(data) == 2:
            # Chuyển đổi giá trị `angle` thành float và `distance` thành int
            try:
                angle = float(data[0])
                distance = int(data[1])
            except ValueError:
                print("Dữ liệu không hợp lệ:", data)
                return

            # Kiểm tra và dự đoán dựa trên khoảng cách distance
            if distance < 88:
                prediction = 0  # Nếu distance < 88 cm, gán 0 (không có va chạm)
            else:
                # Tạo DataFrame với cả `angle` và `distance`
                df = pd.DataFrame([{'angle': angle, 'distance': distance}])
                prediction = model.predict(df[['angle', 'distance']])[0]

            # In ra kết quả dự đoán: 1 hoặc 0
            if prediction == 1:
                print("Collision detected!")
                ws.send("collision_detected")  # Gửi thông báo khi có va chạm
            else:
                print("No collision.")
        else:
            print("Dữ liệu không hợp lệ:", message)

    except Exception as e:
        print(f"Đã xảy ra lỗi khi xử lý dữ liệu: {e}")

# Hàm WebSocket kết nối đến server và nhận dữ liệu
def on_open(ws):
    print("Kết nối WebSocket thành công")

def on_close(ws):
    print("Đã ngắt kết nối WebSocket")

# Hàm WebSocket kết nối đến server
def connect_to_websocket():
    ws_url = "ws://localhost:8080"  # Địa chỉ server WebSocket
    ws = websocket.WebSocketApp(ws_url, on_message=on_message, on_open=on_open, on_close=on_close)
    ws.run_forever()

# Bắt đầu kết nối WebSocket và nhận dữ liệu
if __name__ == "__main__":
    connect_to_websocket()
