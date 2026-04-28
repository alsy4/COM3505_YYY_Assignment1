from flask import Flask, render_template_string, request, jsonify
from datetime import datetime
from collections import deque
import json

app = Flask(__name__)

# latest data from ESP32
sensor_data = {
    "temperature": 0.0,
    "state": "LED_BLINK",
    "last_update": "--",
    "email": "--"
}

HISTORY_SIZE = 60
temp_history = deque(maxlen=HISTORY_SIZE)  # entries: (label_str, temperature_float)

HTML_TEMPLATE = '''
<!DOCTYPE html>
<html>
<head>
    <title>PerfectNoodle - Kitchen Safety Monitor</title>
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
            <span id="temp">{{ temperature }}</span><span class="temp-unit">°C</span>
        </div>

        <div id="status" class="status {{ status_class }}">
            {{ status_text }}
        </div>

        <div class="info">
            <div class="info-item"><strong>Current LED Pattern:</strong> <span id="state">{{ state }}</span></div>
            <div class="info-item"><strong>Safety Threshold:</strong> 17.0°C</div>
            <div class="info-item"><strong>Student Email:</strong> <span id="email">{{ email }}</span></div>
        </div>

        <div class="timestamp">
            Last update: <span id="last-update">{{ last_update }}</span>
        </div>

        <div style="margin: 20px 0;">
            <canvas id="temp-chart" height="180"></canvas>
        </div>

        <div style="margin-top: 20px; font-size: 12px; color: #999;">
            <p>How it works:</p>
            <p>Below 17°C: SOLID Green (Normal)</p>
            <p>Above 17°C: BLINK (Warning) → CHASE (5s) → FLICKER (Emergency)</p>
            <p>Press and Hold button on ESP32 to reset from emergency</p>
        </div>
    </div>

    <script src="https://cdn.jsdelivr.net/npm/chart.js@4.4.1/dist/chart.umd.min.js"></script>
    <script>
    let tempChart = null;

    function initChart() {
        const ctx = document.getElementById('temp-chart').getContext('2d');
        tempChart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: [],
                datasets: [{
                    label: 'Temperature (°C)',
                    data: [],
                    borderColor: '#e64a19',
                    backgroundColor: 'rgba(230, 74, 25, 0.15)',
                    borderWidth: 2,
                    pointRadius: 2,
                    tension: 0.25,
                    fill: true,
                }],
            },
            options: {
                responsive: true,
                animation: false,
                plugins: { legend: { display: false } },
                scales: {
                    x: { ticks: { maxTicksLimit: 6, autoSkip: true } },
                    y: { beginAtZero: false, title: { display: true, text: '°C' } },
                },
            },
        });
    }

    async function refreshHistory() {
        try {
            const r = await fetch('/api/history', {cache: 'no-store'});
            if (!r.ok) return;
            const h = await r.json();
            tempChart.data.labels = h.labels;
            tempChart.data.datasets[0].data = h.values;
            tempChart.update('none');
        } catch (e) { /* keep last good chart */ }
    }

    async function refresh() {
        try {
            const r = await fetch('/api/data', {cache: 'no-store'});
            if (!r.ok) return;
            const d = await r.json();
            document.getElementById('temp').textContent = Number(d.temperature).toFixed(1);
            document.getElementById('state').textContent = (d.state || '').replace('LED_', '');
            document.getElementById('email').textContent = d.email;
            document.getElementById('last-update').textContent = d.last_update;
            const status = document.getElementById('status');
            status.className = 'status ' + d.status_class;
            status.textContent = d.status_text;
        } catch (e) { /* keep last good values */ }
        refreshHistory();
    }

    setInterval(refresh, 1000);
    window.addEventListener('load', () => { initChart(); refresh(); });
    </script>
</body>
</html>
'''

STATUS_BY_STATE = {
    'LED_SOLID':   ('status-safe',     'SAFE - Normal Condition'),
    'LED_BLINK':   ('status-warning',  'WARNING - Temperature Rising!'),
    'LED_CHASE':   ('status-warning',  'ALERT - Reduce Heat!'),
    'LED_FLICKER': ('status-critical', 'EMERGENCY - TURN OFF STOVE NOW!'),
}

def status_for(state):
    return STATUS_BY_STATE.get(state, ('status-safe', 'Unknown State'))

@app.route('/')
def index():
    """Serve the main web page"""
    status_class, status_text = status_for(sensor_data['state'])

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
        now = datetime.now().strftime("%H:%M:%S")
        sensor_data['temperature'] = temperature
        sensor_data['last_update'] = now

        if state:
            sensor_data['state'] = state
        if email:
            sensor_data['email'] = email

        temp_history.append((now, temperature))

        print(f"ESP32: {temperature}°C | State: {state} | Email: {email}")

        return "OK", 200
    else:
        return "No temperature data", 400

@app.route('/api/data')
def get_api_data():
    """JSON endpoint consumed by the dashboard's polling script."""
    status_class, status_text = status_for(sensor_data['state'])
    return jsonify({
        **sensor_data,
        'status_class': status_class,
        'status_text': status_text,
    })

# This will store the live temperature history (not stored locally)
@app.route('/api/history')
def get_api_history():
    """Time-series buffer of the most recent ESP32 readings."""
    return jsonify({
        'labels': [t for t, _ in temp_history],
        'values': [v for _, v in temp_history],
    })

if __name__ == '__main__':
    print("Flask Server Starting...")
    print(f"Web: http://localhost:9194")
    
    app.run(host='0.0.0.0', port=9194, debug=True)