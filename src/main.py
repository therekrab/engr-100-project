import cv2

import aruco_identify
import two_stream
import updated_line_follow

line_follow_ip = "192.168.50.204"
line_follow_stream_source = f"http://{line_follow_ip}/stream"

marker_ip = "192.168.50.203"
marker_stream_source = f"http://{marker_ip}/stream"

controller_ip = line_follow_ip
controller_port = 5005


def main():
    line_follow_stream = two_stream.ThreadedVideoStream(line_follow_stream_source)
    print("started line following stream")
    marker_stream = two_stream.ThreadedVideoStream(marker_stream_source)
    print("started marker stream")

    controller = two_stream.MultiController(two_stream.SocketController(controller_ip, controller_port), [two_stream.DebugController()])
    # controller = two_stream.DebugController()

    # automatic configuration is so much nicer than re-deploying code each and every time the rover runs.
    controller.send("P0.9,0.0,0.25")

    while True:
        # start of loop, check if either friend has a new value for us:
        if line_follow_stream.is_stopped():
            print("Line following stream stopped.")
            break
        if marker_stream.is_stopped():
            print("Marker stream stopped.")
            break

        if line_follow_stream.has_new_data():
            line_follow_frame = line_follow_stream.read()
            updated_line_follow.update(line_follow_frame, controller)

        if marker_stream.has_new_data():
            marker_frame = marker_stream.read()
            aruco_identify.update(marker_frame, controller)

        if cv2.waitKey(1) & 0xFF == ord("q"):
            controller.send("LSTOP")
            break

    cv2.destroyAllWindows()
    line_follow_stream.stop()
    marker_stream.stop()


if __name__ == "__main__":
    main()
