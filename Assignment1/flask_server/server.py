from flask import Flask, render_template_string, request
from datetime import datetime
import json

app = Flask(__name__)

# latest data from ESP32
sensor_data = {
    "temperature": 0.0,
    "state": "LED_BLINK",
    "last_update": "--",
    "email": "--"
}

HTML_TEMPLATE = '''
<!DOCTYPE html>
<html>
<head>
    <title>PerfectNoodle - Kitchen Safety Monitor</title>
    <meta http-equiv="refresh" content="2">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body {
            font-family: 'Times New Roman', serif;
            text-align: center;
            position: relative;
            margin: 0;
            padding: 20px;
            min-height: 100vh;
        }
        body::before {
            content: "";
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            background-image: url('/static/background.jpg');
            background-size: cover;
            background-position: center;
            background-repeat: no-repeat;
            background-attachment: fixed;
            opacity: 0.5;
            z-index: -1;
        }
        .container {
            max-width: 600px;
            margin: 0 auto;
            background: white;
            padding: 30px;
            opacity: 0.83;
        }
        h1 {
            color: #24325;
            margin-bottom: 10px;
        }
        .temp {
            font-size: 72px;
            font-weight: bold;
            color: #e64a19;
            margin: 20px 0;
        }
        .temp-unit {
            font-size: 32px;
        }
        .status {
            margin: 20px 0;
            font-weight: bold;
            font-size: 18px;
        }
        .status-safe {
            background: #4CAF50;
            color: white;
        }
        .status-warning {
            background: #FF9800;
            color: white;
            animation: pulse 1s infinite;
        }
        .status-critical {
            background: #f44336;
            color: white;
            animation: blink 0.5s infinite;
        }
        .info {
            background: #f0f0f0;
            margin: 20px 0;
            text-align: left;
        }
        .info-item {
            margin: 10px 0;
        }
        .timestamp {
            color: grey;
            font-size: 12px;
            margin-top: 20px;
        }
        @keyframes pulse {
            0%, 100% { opacity: 1; }
            50% { opacity: 0.7; }
        }
        @keyframes blink {
            0%, 100% { opacity: 1; }
            50% { opacity: 0.5; }
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>PerfectNoodle Safety System</h1>
        <p>Real-time stove temperature</p>
        
        <div class="temp">
            {{ temperature }}<span class="temp-unit">°C</span>
        </div>
        
        <div class="status {{ status_class }}">
            {{ status_text }}
        </div>
        
        <div class="info">
            <div class="info-item"><strong>Current LED Pattern:</strong> {{ state }}</div>
            <div class="info-item"><strong>Safety Threshold:</strong> 17.0°C</div>
            <div class="info-item"><strong>Student Email:</strong> {{ email }}</div>
        </div>
        
        <div class="timestamp">
            Last update: {{ last_update }}
        </div>
        
        <div style="margin-top: 20px; font-size: 12px; color: #999;">
            <p>How it works:</p>
            <p>Below 17°C: SOLID Green (Normal)</p>
            <p>Above 17°C: BLINK (Warning) → CHASE (5s) → FLICKER (Emergency)</p>
            <p>Press and Hold button on ESP32 to reset from emergency</p>
        </div>
    </div>
</body>
</html>
'''

@app.route('/')
def index():
    """Serve the main web page"""
    # status and text based on LED state
    state = sensor_data['state']
    
    if state == 'LED_SOLID':
        status_class = 'status-safe'
        status_text = 'SAFE - Normal Condition'
    elif state == 'LED_BLINK':
        status_class = 'status-warning'
        status_text = 'WARNING - Temperature Rising!'
    elif state == 'LED_CHASE':
        status_class = 'status-warning'
        status_text = 'ALERT - Reduce Heat!'
    elif state == 'LED_FLICKER':
        status_class = 'status-critical'
        status_text = 'EMERGENCY - TURN OFF STOVE NOW!'
    else:
        status_class = 'status-safe'
        status_text = 'Unknown State'
    
    return render_template_string(
        HTML_TEMPLATE,
        temperature=round(sensor_data['temperature'], 1),
        state=sensor_data['state'].replace('LED_', ''),
        status_class=status_class,
        status_text=status_text,
        last_update=sensor_data['last_update'],
        email=sensor_data['email']
    )

@app.route('/sensor')
def receive_sensor_data():
    """Endpoint for ESP32 to send sensor data"""
    # get parameters from ESP32
    temperature = request.args.get('temperature', type=float)
    state = request.args.get('state', type=str)
    email = request.args.get('email', type=str)
    
    if temperature is not None:
        sensor_data['temperature'] = temperature
        sensor_data['last_update'] = datetime.now().strftime("%H:%M:%S")
        
        if state:
            sensor_data['state'] = state
        if email:
            sensor_data['email'] = email
        
        print(f"ESP32: {temperature}°C | State: {state} | Email: {email}")
        
        return "OK", 200
    else:
        return "No temperature data", 400

@app.route('/api/data')
def get_api_data():
    """JSON endpoint for potential AJAX use"""
    return jsonify(sensor_data)

if __name__ == '__main__':
    print("Flask Server Starting...")
    print(f"Web: http://localhost:9194")
    
    app.run(host='0.0.0.0', port=9194, debug=True)