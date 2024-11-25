#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <WiFi.h>
#include <WebServer.h>

// Wi-Fi credentials
const char* ssid = "iQOO Neo7 Pro";         // Replace with your Wi-Fi SSID
const char* password = "REX@24rox";  // Replace with your Wi-Fi password

// Web server on port 80
WebServer server(80);

// Motor pins
int motor1Pin1 = 25; 
int motor1Pin2 = 26; 
int motor2Pin1 = 19; 
int motor2Pin2 = 23;

// Ultrasonic sensor pins
const int trigPin = 5;
const int echoPin = 18;

#define SOUND_SPEED 0.034 // cm/μs
const float OBSTACLE_THRESHOLD = 30.0; // Threshold distance to stop (cm)

long duration;
float distanceCm;
String task_code = "";
bool isMoving = false;

// I2C pins for MPU6050
#define SDA_PIN 21
#define SCL_PIN 22

// Tyre properties
const float TYRE_RADIUS_CM = 4.0; // 4 cm radius
const float WHEEL_CIRCUMFERENCE_CM = 2 * PI * TYRE_RADIUS_CM;

// Create MPU6050 object
Adafruit_MPU6050 mpu;

// Variables for wheel speed and orientation
float speedLeftWheel = 0;
float speedRightWheel = 0;
float orientation = 0;

// LED Pin (blue LED on ESP32)
const int ledPin = 2;  // Default GPIO 2 for blue LED

// Function to initialize motors
void setupMotors() {
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
}

// Function to initialize ultrasonic sensor
void setupUltrasonicSensor() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

// Function to initialize MPU6050
void setupMPU6050() {
  Wire.begin(SDA_PIN, SCL_PIN);

  if (!mpu.begin()) {
    Serial.println("Failed to initialize MPU6050!");
    while (1) {
      delay(10);
    }
  }

  Serial.println("MPU6050 initialized successfully!");
}

// Function to calculate distance using ultrasonic sensor
float calculateDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  return duration * SOUND_SPEED / 2;
}

// Function to stop the motors
void stopMotor() {
  analogWrite(motor1Pin1, 0);
  analogWrite(motor1Pin2, 0);
  analogWrite(motor2Pin1, 0);
  analogWrite(motor2Pin2, 0);
  isMoving = false;
  Serial.println("Motor stopped due to obstacle.");
}

// Function to move forward
void moveForward() {
  analogWrite(motor1Pin1, 0);
  analogWrite(motor1Pin2, 255);
  analogWrite(motor2Pin1, 255);
  analogWrite(motor2Pin2, 0);
  isMoving = true;
  Serial.println("Moving forward.");
}

// Function to move backward
void moveBackward() {
  analogWrite(motor1Pin1, 255);
  analogWrite(motor1Pin2, 0);
  analogWrite(motor2Pin1, 0);
  analogWrite(motor2Pin2, 255);
  isMoving = true;
  Serial.println("Moving backward.");
}

// Function to calculate wheel speed using MPU6050 (gyroscope)
void calculateWheelSpeed() {
  sensors_event_t accel, gyro, temp;
  mpu.getEvent(&accel, &gyro, &temp);

  // Assume the gyroscope's Z-axis measures angular velocity of the wheels
  // gyro.gyro.z gives angular velocity in rad/s; convert to cm/s
  float angularVelocityLeft = gyro.gyro.z;  // Simulate left wheel
  float angularVelocityRight = -gyro.gyro.z; // Simulate right wheel

  // Convert angular velocity to linear speed (v = ω * r)
  speedLeftWheel = angularVelocityLeft * TYRE_RADIUS_CM;
  speedRightWheel = angularVelocityRight * TYRE_RADIUS_CM;

  Serial.print("Speed Left Wheel (cm/s): ");
  Serial.println(speedLeftWheel);
  Serial.print("Speed Right Wheel (cm/s): ");
  Serial.println(speedRightWheel);
}

// Function to calculate orientation using MPU6050 (gyroscope)
void calculateOrientation() {
  sensors_event_t accel, gyro, temp;
  mpu.getEvent(&accel, &gyro, &temp);

  // Integrate gyroscope's angular velocity to calculate orientation (yaw)
  // gyro.gyro.z gives angular velocity in rad/s
  float deltaTime = 0.1; // Assume loop runs every 100ms
  orientation += gyro.gyro.z * deltaTime;

  Serial.print("Orientation (radians): ");
  Serial.println(orientation);
}

// Function to handle root HTTP request
void handleRoot() {
  digitalWrite(ledPin, HIGH);  // Turn on the LED during data transfer
  server.send(200, "text/plain", "ESP32 is ready to receive commands.");
  digitalWrite(ledPin, LOW);   // Turn off the LED after responding
}

// Function to handle task command HTTP request
void handleCommand() {
  digitalWrite(ledPin, HIGH);  // Turn on the LED during data transfer
  if (server.hasArg("task")) {
    task_code = server.arg("task");
    server.send(200, "text/plain", "Command received: " + task_code);
  } else {
    server.send(400, "text/plain", "Task code not provided.");
  }
  digitalWrite(ledPin, LOW);   // Turn off the LED after responding
}

// Setup function
void setup() {
  Serial.begin(115200);

  // Initialize motors, ultrasonic sensor, and MPU6050
  setupMotors();
  setupUltrasonicSensor();
  setupMPU6050();

  // Initialize the blue LED
  pinMode(ledPin, OUTPUT);

  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Configure HTTP server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/command", HTTP_POST, handleCommand);

  // Start HTTP server
  server.begin();
  Serial.println("HTTP server started.");
}

// Loop function
void loop() {
  // Handle incoming HTTP requests
  server.handleClient();

  // Check for obstacles
  distanceCm = calculateDistance();
  if (isMoving && distanceCm < OBSTACLE_THRESHOLD) {
    stopMotor();
    Serial.println("Obstacle detected!");
  }

  // Execute commands based on the received task code
  if (!task_code.isEmpty()) {
    if (task_code == "1") {
      moveForward();
    } else if (task_code == "2") {
      moveBackward();
    } else if (task_code == "7") {
      stopMotor();
    }

    // Reset task code after execution
    task_code = "";
  }

  // Debugging: Print distance
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);

  // Calculate wheel speed and orientation
  calculateWheelSpeed();
  calculateOrientation();

  delay(100); // Small delay for sensor stabilization
}
