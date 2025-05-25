from flask import Flask, request, jsonify
import requests

app = Flask(__name__)

pending_payload = None
latest_location = {"lat": None, "lng": None}
latest_battery = None 

GOOGLE_API_KEY = "AIzaSyDczVkoau31S8n6u-w9nDOUHB4RuvVUhu4"
LORIOT_API_URL = "https://iotnet.teracom.dk/1/rest"
LORIOT_API_TOKEN = "vnoWTwAAABFpb3RuZXQudGVyYWNvbS5kazJLJbbGXfMHJzq7INWQkpA="
DEVICE_EUI = "0004A30B010651F7"

def decode_coord_val_hex(bytes3, min_val, max_val):
    normalized = (bytes3[0] << 16) | (bytes3[1] << 8) | bytes3[2]
    val = (normalized / 16777215.0) * (max_val - min_val) + min_val
    return val

def parse_gps_data(hex_data):
    bytes_data = bytearray.fromhex(hex_data)
    if len(bytes_data) < 7:
        print("GPS payload too short:", len(bytes_data))
        return None, None
    lat = decode_coord_val_hex(bytes_data[1:4], -90.0, 90.0)
    lng = decode_coord_val_hex(bytes_data[4:7], -90.0, 90.0)
    return lat, lng

def parse_wifi_data(hex_data):
    bytes_data = bytearray.fromhex(hex_data)
    if len(bytes_data) < 9:
        return []
    wifi_networks = []
    index = 1
    while index + 8 <= len(bytes_data):
        block = bytes_data[index:index+8]
        bssid = ':'.join(f"{b:02x}" for b in block[0:6])
        rssi = -block[6]
        channel = block[7]
        wifi_networks.append({"bssid": bssid, "rssi": rssi, "channel": channel})
        index += 8
    return wifi_networks

def parse_battery_data(hex_data):
    global latest_battery
    bytes_data = bytearray.fromhex(hex_data)
    if len(bytes_data) < 2:
        print("Battery payload too short:", len(bytes_data))
        return None
    if bytes_data[0] != 0x30:
        print("Not a battery payload (expected 0x30)")
        return None
    battery_level = bytes_data[1]
    latest_battery = battery_level
    print(f"Battery level: {battery_level}%")
    return battery_level

def get_coordinates_from_wifi(wifi_networks):
    url = f"https://www.googleapis.com/geolocation/v1/geolocate?key={GOOGLE_API_KEY}"
    payload = {
        "wifiAccessPoints": [
            {
                "macAddress": net["bssid"],
                "signalStrength": net["rssi"],
                "channel": net["channel"]
            } for net in wifi_networks
        ]
    }
    try:
        response = requests.post(url, json=payload)
        response.raise_for_status()
        location = response.json().get("location", {})
        return location.get("lat"), location.get("lng")
    except Exception as e:
        print("Error using Google API:", e)
        return None, None

def send_downlink_to_loriot(payload_hex):
    headers = {
        "Authorization": f"Bearer {LORIOT_API_TOKEN}",
        "Content-Type": "application/json"
    }
    json_data = {
        "cmd": "tx",
        "EUI": DEVICE_EUI,
        "port": 1,
        "confirmed": True,
        "priority": 2,
        "data": payload_hex,
    }
    try:
        response = requests.post(LORIOT_API_URL, json=json_data, headers=headers)
        print("Response from Loriot:", response.status_code, response.text)
        return response.status_code == 200
    except Exception as e:
        print("Error during downlink:", e)
        return False

@app.route("/send-to-lorawan", methods=["POST"])
def receive_from_phone():
    data = request.json
    if not data or "payload" not in data:
        return jsonify({"error": "Missing payload"}), 400
    payload = data["payload"]
    print("Received payload from phone:", payload)
    success = send_downlink_to_loriot(payload)
    return jsonify({"status": "Downlink sent"}) if success else jsonify({"error": "Failed"}), 200

@app.route("/loriot-uplink", methods=["POST"])
def handle_loriot_uplink():
    global latest_location
    data = request.json
    print("Uplink received from Loriot:", data)
    hex_data = data.get("data")
    if hex_data and len(hex_data) >= 2:
        type_byte = int(hex_data[0:2], 16)
        if type_byte == 0x10:
            print("GPS data detected")
            lat, lng = parse_gps_data(hex_data)
            if lat and lng:
                latest_location = {"lat": lat, "lng": lng, "source": "gps"}
                print("GPS coordinates:", latest_location)
        elif type_byte == 0x20:
            print("WiFi data detected")
            wifi_data = parse_wifi_data(hex_data)
            print("Detected networks:", wifi_data)
            lat, lng = get_coordinates_from_wifi(wifi_data)
            if lat and lng:
                latest_location = {"lat": lat, "lng": lng, "source": "wifi"}
                print("Coordinates from WiFi:", latest_location)
        elif type_byte == 0x30:
            print("Battery data detected")
            parse_battery_data(hex_data)
        else:
            print(f"Unknown type byte: {type_byte}")
    return jsonify({})

@app.route("/location", methods=["GET"])
def get_latest_location():
    return jsonify(latest_location)

@app.route("/battery", methods=["GET"])
def get_latest_battery():
    return jsonify({"battery": latest_battery})

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5050)
