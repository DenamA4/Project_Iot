const WebSocket = require('ws');

// Tạo WebSocket server lắng nghe trên cổng 8080
const wss = new WebSocket.Server({ port: 8080 });

const clients = new Set(); // Sử dụng Set để lưu trữ các kết nối client

wss.on('connection', function connection(ws) {
    console.log('Client connected');
    clients.add(ws); // Thêm client mới vào Set

    // Xử lý tin nhắn từ client
    ws.on('message', function incoming(message) {
        const data = message.toString();
        console.log(`Received from client: ${data}`);

        // Gửi dữ liệu đến tất cả client khác
        clients.forEach(client => {
            if (client !== ws && client.readyState === WebSocket.OPEN) {
                client.send(data);
            }
        });
    });

    // Xử lý khi client ngắt kết nối
    ws.on('close', function() {
        console.log('Client disconnected');
        clients.delete(ws); // Xóa client khỏi Set
    });
});

console.log('WebSocket server is running on ws://localhost:8080');
