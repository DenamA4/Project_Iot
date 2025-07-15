import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.linear_model import LogisticRegression
import joblib

# Đọc dữ liệu từ file Excel
def read_data_from_excel(file_path):
    df = pd.read_excel(file_path)
    if 'Message' in df.columns:
        df[['angle', 'distance']] = df['Message'].str.split(',', expand=True)
        df['angle'] = pd.to_numeric(df['angle'].str.strip(), errors='coerce')  # Chuyển thành kiểu số
        df['distance'] = pd.to_numeric(df['distance'].str.strip(), errors='coerce')  # Chuyển thành kiểu số
    df = df.dropna(subset=['angle', 'distance'])
    return df

# Hàm huấn luyện mô hình
def train_model(file_path):
    # Đọc dữ liệu
    df = read_data_from_excel(file_path)
    
    # Đặt 'angle' và 'distance' làm biến đầu vào (X), và 'label' là đầu ra (y)
    X = df[['angle', 'distance']]
    y = df['Label']

    # Chia dữ liệu thành tập huấn luyện và tập kiểm tra
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

    # Huấn luyện mô hình Logistic Regression
    model = LogisticRegression()
    model.fit(X_train, y_train)

    # Lưu mô hình đã huấn luyện
    joblib.dump(model, 'D:\AI_NCKH\collision_model.pkl')


# Huấn luyện mô hình từ dữ liệu
train_model(r'D:\AI_NCKH\test.xlsx')
