import cv2
import numpy as np

import two_stream

low_orange = np.array([0, 0, 20], dtype=np.uint8)
high_orange = np.array([70, 255, 255], dtype=np.uint8)
angle_weight = 40

def update(frame, controller: two_stream.Controller):
    # Blur the frame to reduce noise
    frame = cv2.GaussianBlur(frame, (5, 5), 0)

    # Detect orange tape in HSV
    hsv_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    gray_threshold = cv2.inRange(
        hsv_frame,
        low_orange,
        high_orange
    )

    # Fill small holes in detection
    hole_kernel = np.ones((5, 5), np.uint8)
    gray_threshold = cv2.morphologyEx(gray_threshold, cv2.MORPH_CLOSE, hole_kernel)

    # Detect contours
    gray_threshold_contours, _ = cv2.findContours(gray_threshold, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)

    # Take the largest contour. Filters out some false positives
    if gray_threshold_contours:
        bestContour = max(gray_threshold_contours, key=cv2.contourArea)
        if cv2.contourArea(bestContour) == 0:
            bestContour = None
    else:
        bestContour = None

    # Get frame center
    center_of_frame = np.array([frame.shape[1] / 2, frame.shape[0] / 2], int)

    try:
        if bestContour is not None:
            # Fit line to contour
            mask = np.zeros_like(gray_threshold)
            cv2.drawContours(mask, [bestContour], -1, 255, -1)
            points = []
            for y in range(frame.shape[0]):
                xs = np.where(mask[y] > 0)[0]
                if len(xs) > 0:
                    x = (xs[0] + xs[-1]) / 2
                    points.append((x, y))

            points = np.array(points)
            slope, intercept = np.polyfit(points[:, 1], points[:, 0], 1)

            # Offset error
            y_ref = frame.shape[0] / 2
            x_ref = y_ref * slope + intercept
            offset_error = center_of_frame[0] - x_ref

            # Angle error (slope relative to vertical)
            angle_error = slope

            # Combined error
            error = offset_error + angle_weight * angle_error

            # Draw detected contour
            cv2.putText(frame, "Line: FOUND", (0, 20), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)
            cv2.drawContours(frame, [bestContour], -1, (0, 255, 0), 2)

            # Draw fitted line across image
            y1 = 0
            x1 = int(y1 * slope + intercept)
            y2 = frame.shape[0] - 1
            x2 = int(y2 * slope + intercept)
            cv2.line(frame, (x1, y1), (x2, y2), (255, 0, 255), 2)

            # Display errors
            cv2.putText(frame, f"Offset error: {offset_error:.1f}  Angle error: {angle_error:.2f}", (0, 45), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255))
            cv2.putText(frame, f"Combined error: {error:.1f}", (0, 65), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255))

            controller.send(f"L{error}")
        else:
            cv2.putText(frame, "Line: NOT FOUND", (0, 20), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 2)
    except Exception as e:  # noqa: BLE001
        print(f"Exception incurred: {e}")
        controller.send("STOP")

    # Draw the x-axis median line and frame center
    cv2.circle(frame, (center_of_frame[0], center_of_frame[1]), 7, (255, 255, 255), -1)
    cv2.line(frame, (center_of_frame[0], 0), (center_of_frame[0], frame.shape[0]), (0, 255, 255), 2)

    # Display the current video frame
    cv2.imshow("Current Line-Following Output", frame)
