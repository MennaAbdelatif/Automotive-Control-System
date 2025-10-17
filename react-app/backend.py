from flask import Flask, jsonify
import paho.mqtt.client as mqtt
from datetime import datetime
from flask_cors import CORS

# ---------------------------
# Flask App Setup
# ---------------------------
app = Flask(__name__)
CORS(app)  # Allow access from your React dashboard (localhost:5173)

# ---------------------------
# Data Storage (in memory)
# ---------------------------
metrics = {
    "totalAccidents": 0,
    "activeCars": 1,
    "alertsSent": 0
}

accidents = []
chart_data = []

# ---------------------------
# MQTT Setup
# ---------------------------
BROKER = "test.mosquitto.org"  # Public MQTT broker
PORT = 1883
TOPIC_ALERT = "car/alert"
TOPIC_LIVE = "car/location"

def on_connect(client, userdata, flags, rc):
    print("Connected to MQTT broker with code:", rc)
    # Subscribe to both accident and live location topics
    client.subscribe([(TOPIC_ALERT, 0), (TOPIC_LIVE, 0)])

def on_message(client, userdata, msg):
    """This function runs every time a message is received on MQTT."""
    global metrics, accidents, chart_data
    data = msg.payload.decode(errors="ignore")
    time_now = datetime.now().strftime("%H:%M:%S")

    if msg.topic == TOPIC_ALERT:
        # Accident alert message from Raspberry Pi
        metrics["totalAccidents"] += 1
        metrics["alertsSent"] += 1
        accidents.append({
            "id": len(accidents) + 1,
            "carId": "CAR001",
            "time": time_now,
            "location": data
        })
        chart_data.append({
            "time": time_now,
            "accidents": metrics["totalAccidents"]
        })
        print(" Accident received:", data)

    elif msg.topic == TOPIC_LIVE:
        # Live location update message
        metrics["activeCars"] = 1
        print(" Live location:", data)

# Initialize MQTT client
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

print(" Connecting to MQTT broker...")
client.connect(BROKER, PORT, 60)
client.loop_start()  # Keep listening in background

# ---------------------------
# API Routes (for dashboard)
# ---------------------------
@app.route("/api/metrics")
def get_metrics():
    return jsonify(metrics)

@app.route("/api/accidents")
def get_accidents():
    # Return last 10 accidents
    return jsonify(accidents[-10:])

@app.route("/api/chart")
def get_chart():
    # Return last 10 points for chart
    return jsonify(chart_data[-10:])

# ---------------------------
# Run the Flask Server
# ---------------------------
if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
