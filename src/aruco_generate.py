import cv2
import numpy as np

selected_dictionary = cv2.aruco.DICT_4X4_50

def make_tag(id):
    tag = np.ones((400, 400, 1), dtype="uint8")

    cv2.aruco.generateImageMarker(
        cv2.aruco.getPredefinedDictionary(selected_dictionary),
        id,
        sidePixels=400,
        img=tag,
    )

    border_width = 100

    bordered = cv2.copyMakeBorder(
            src=tag,
            top=border_width,
            left=border_width,
            right=border_width,
            bottom=border_width,
            value=(255,255,255),
            borderType=cv2.BORDER_CONSTANT,
            )

    cv2.imwrite(f"generated-tags/aruco_marker_{id}.png", bordered)

for id in range(1, 20):
    make_tag(id)
