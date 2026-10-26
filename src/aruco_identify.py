import sys
import time

import cv2

front_cam_stream_url = "http://192.168.50.203/stream"

aruco_dict = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_4X4_50)

detector_params = cv2.aruco.DetectorParameters()
detector = cv2.aruco.ArucoDetector(aruco_dict, detector_params)

vid = cv2.VideoCapture(front_cam_stream_url)

with open("timing_data.csv", "w") as timing_file:
    timing_file.write("capture_elapsed,detected_elapsed")
    while True:
        start = time.time()
        if not vid.isOpened():
            break
        ret, frame = vid.read()
        captured_elapsed = time.time() - start

        if not ret:
            print("Error reading frame")
            sys.exit(1)

        corners, ids, rejected = detector.detectMarkers(frame)
        detected_elapsed = time.time() - start

        if ids is not None:
            cv2.aruco.drawDetectedMarkers(frame, corners, ids)

        cv2.imshow("Stream", frame)
        if cv2.waitKey(1) & 0xFF == ord("q"):
            break
        timing_file.write(f"{captured_elapsed},{detected_elapsed}\n")
    cv2.destroyAllWindows()
