// BluetoothModule.js
class BluetoothModule {
    constructor() {
        this.device = null;
        this.server = null;
        this.characteristic = null;
        this.statusElement = document.getElementById('status');
        this.connectBtn = document.getElementById('connectBtn');
        this.disconnectBtn = document.getElementById('disconnectBtn');
        this.sendBtn = document.getElementById('sendBtn');
        this.messageInput = document.getElementById('messageInput');
        this.dataDisplay = document.getElementById('dataDisplay');

        // 绑定事件
        this.connectBtn.addEventListener('click', () => this.connect());
        this.disconnectBtn.addEventListener('click', () => this.disconnect());
        this.sendBtn.addEventListener('click', () => this.sendMessage());
    }

    // 更新状态显示
    updateStatus(message) {
        this.statusElement.textContent = `状态: ${message}`;
    }

    // 添加接收到的数据到显示区域
    appendData(data) {
        this.dataDisplay.textContent += `${data}\n`;
        this.dataDisplay.scrollTop = this.dataDisplay.scrollHeight;
    }

    // 连接蓝牙设备
    async connect() {
        try {
            this.updateStatus('正在扫描设备...');

            // 请求蓝牙设备（可添加过滤条件，如服务UUID）
            this.device = await navigator.bluetooth.requestDevice({
                acceptAllDevices: true, // 接受所有设备（生产环境应指定过滤条件）
                optionalServices: ['0000ffe0-0000-1000-8000-00805f9b34fb'] // 示例UUID（根据设备修改）
            });

            this.device.addEventListener('gattserverdisconnected', () => {
                this.updateStatus('设备已断开');
                this.disconnectBtn.disabled = true;
                this.sendBtn.disabled = true;
            });

            this.updateStatus('正在连接服务器...');
            this.server = await this.device.gatt.connect();

            this.updateStatus('正在获取服务...');
            const service = await this.server.getPrimaryService('0000ffe0-0000-1000-8000-00805f9b34fb');

            this.updateStatus('正在获取特征值...');
            this.characteristic = await service.getCharacteristic('0000ffe1-0000-1000-8000-00805f9b34fb');

            // 启用特征值变化通知（如果设备支持）
            try {
                await this.characteristic.startNotifications();
                this.characteristic.addEventListener('characteristicvaluechanged', (event) => {
                    const value = event.target.value;
                    const decoder = new TextDecoder('GB2312'); //不是UTF-8搞了半天！！！
                    this.appendData(`收到: \n${decoder.decode(value)}`);
                });
                this.appendData('已启用数据接收通知');
            } catch (error) {
                this.appendData('警告: 设备不支持数据通知，需手动读取');
            }

            this.updateStatus('已连接');
            this.connectBtn.disabled = true;
            this.disconnectBtn.disabled = false;
            this.sendBtn.disabled = false;
            this.appendData(`已连接设备: ${this.device.name || '未知设备'}`);

        } catch (error) {
            this.updateStatus('连接失败');
            this.appendData(`错误: ${error.message}`);
        }
    }

    // 断开蓝牙连接
    async disconnect() {
        if (!this.device) return;

        try {
            if (this.characteristic) {
                try {
                    await this.characteristic.stopNotifications();
                } catch (error) {
                    console.warn('停止通知失败:', error);
                }
            }
            if (this.server) {
                await this.server.disconnect();
            }
            this.updateStatus('已断开');
            this.appendData('连接已关闭');
        } catch (error) {
            this.appendData(`断开错误: ${error.message}`);
        } finally {
            this.device = null;
            this.server = null;
            this.characteristic = null;
            this.connectBtn.disabled = false;
            this.disconnectBtn.disabled = true;
            this.sendBtn.disabled = true;
        }
    }

    // 发送消息到蓝牙设备
    async sendMessage() {
        /*
        if (!this.characteristic) {
            this.appendData('错误: 未连接设备');
            return;
        }

        const message = this.messageInput.value.trim();
        if (!message) {
            this.appendData('错误: 消息不能为空');
            return;
        }

        try {
            const encoder = new TextEncoder('utf-8');
            const data = encoder.encode(message + "\n");
            //await this.characteristic.writeValue(data);
            // 尝试 writeValueWithoutResponse（透传模式推荐）
            await this.characteristic.writeValueWithoutResponse(data);
            this.appendData(`发送: \n${message}`);
            this.messageInput.value = '';
        } catch (error) {
            this.appendData(`发送失败: ${error.message}`);
            console.error("详细错误:", error);
        }
        */
        if (!this.characteristic) {
            this.appendData('错误: 未连接设备');
            return;
        }

        const message = this.messageInput.value.trim();
        if (!message) {
            this.appendData('错误: 消息不能为空');
            return;
        }

        try {
            // 1. 提取首字符并转为大写（确保硬件指令格式正确）
            const command = message.charAt(0).toUpperCase();
            const validCommands = ['A', 'B', 'C', 'D', 'E', 'F']; // 硬件支持的指令列表

            if (!validCommands.includes(command)) {
                this.appendData(`错误: 无效指令 (支持: ${validCommands.join(',')})`);
                return;
            }

            // 2. 编码指令（仅发送单个字符 + \n）
            const data = new TextEncoder().encode(command);
            console.log("发送的原始数据:", command); // 调试：查看实际发送的字符
            console.log("编码后的字节:", Array.from(data).map(b => b.toString(16).padStart(2, '0'))); // 调试：查看字节

            // 3. 发送数据（优先使用 writeValueWithoutResponse）
            await this.characteristic.writeValueWithoutResponse(data);
            this.appendData(`已发送: ${command}`);
            this.messageInput.value = '';
        } catch (error) {
            this.appendData(`发送失败: ${error.message}`);
            console.error("详细错误:", error);
        }
    }
}

// 初始化蓝牙模块
document.addEventListener('DOMContentLoaded', () => {
    new BluetoothModule();
});