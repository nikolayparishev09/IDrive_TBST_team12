import cv2
import mediapipe as mp
import serial
import time

# Настройка на серийния порт
arduino = serial.Serial("COM6", 115200, timeout=1)
time.sleep(2)

mp_hands = mp.solutions.hands
hands = mp_hands.Hands(max_num_hands=1)
mp_draw = mp.solutions.drawing_utils

cap = cv2.VideoCapture(0)

def finger_up(hand_landmarks, tip, dip):
    return hand_landmarks.landmark[tip].y < hand_landmarks.landmark[dip].y

def detect_rock(hand_landmarks):
    index_up = finger_up(hand_landmarks, 8, 6)
    middle_up = finger_up(hand_landmarks, 12, 10)
    ring_up = finger_up(hand_landmarks, 16, 14)
    pinky_up = finger_up(hand_landmarks, 20, 18)
    if index_up and not middle_up and not ring_up and pinky_up:
        return True
    return False

while True:
    ret, frame = cap.read()
    if not ret:
        break
    frame = cv2.flip(frame, 1)
    rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    results = hands.process(rgb)

    gesture_detected = False

    if results.multi_hand_landmarks:
        for hand_landmarks in results.multi_hand_landmarks:
            mp_draw.draw_landmarks(frame, hand_landmarks, mp_hands.HAND_CONNECTIONS)
            if detect_rock(hand_landmarks):
                gesture_detected = True

    # Изпращане на сигнал към Arduino
    arduino.write(b'G' if gesture_detected else b'R')

    cv2.putText(frame, f"Gesture: {'ROCK' if gesture_detected else 'None'}",
                (10,30), cv2.FONT_HERSHEY_SIMPLEX, 1,
                (0,255,0) if gesture_detected else (0,0,255), 2)

    cv2.imshow("Hand Gesture", frame)
    if cv2.waitKey(1) & 0xFF == 27:
        break

cap.release()
cv2.destroyAllWindows()
