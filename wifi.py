import requests
import time

# Replace with your ESP32's IP address
ESP32_IP = "192.168.1.100"  # Change to your ESP32's actual IP
SERIAL_MONITOR_ENDPOINT = f"http://{ESP32_IP}/serial"

def fetch_serial_output():
    """
    Fetch serial output from the ESP32 and print it.
    """
    try:
        response = requests.get(SERIAL_MONITOR_ENDPOINT)
        if response.status_code == 200:
            output = response.text
            if output.strip():
                print(output.strip())
        else:
            print(f"Failed to fetch serial output. HTTP {response.status_code}: {response.text}")
    except Exception as e:
        print(f"Error fetching serial output: {e}")

def main():
    """
    Continuously fetch and display serial output from the ESP32.
    """
    print(f"Connecting to ESP32 at {ESP32_IP}...")
    print("Fetching serial output. Press Ctrl+C to stop.")
    try:
        while True:
            fetch_serial_output()
            time.sleep(1)  # Poll every second
    except KeyboardInterrupt:
        print("\nStopped fetching serial output.")
    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    main()
