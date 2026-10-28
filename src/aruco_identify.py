import cv2

import two_stream
from timing_capture import log_time

aruco_dict = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_4X4_50)

detector_params = cv2.aruco.DetectorParameters()
detector_params.minMarkerPerimeterRate = 0.1 # require bigger tags in frame than the default.
detector = cv2.aruco.ArucoDetector(aruco_dict, detector_params)

@log_time("marker")
def update(frame, controller: two_stream.Controller):
        corners, ids, _ = detector.detectMarkers(frame)

        if ids is not None:
            cv2.aruco.drawDetectedMarkers(frame, corners, ids)
            controller.send(f"T{ids[0]}")

        cv2.imshow("Detected Markers", frame)
