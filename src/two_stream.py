import sqlite3
import socket
import time
from abc import ABC, abstractmethod
from threading import Lock, Thread

import cv2

POOL_SIZE = 50

class ThreadedVideoStream:

    def __init__(self, src_url, name="camera_stream"):
        self.name = name
        self.stream = cv2.VideoCapture(src_url)
        self.stopped = False
        self.ok, self.frame = self.stream.read()
        self.lock = Lock()
        self.seen_latest = False
        # self.image_pool = [None for _ in range(POOL_SIZE)]
        # self.image_pool_next = 0
        time.sleep(1)
        self.start()

    def start(self):
        t = Thread(target=self.update, args=())
        t.daemon = True
        t.start()

    def update(self):
        while not self.stopped:
            if not self.ok:
                self.stop()
                break
            ok, frame = self.stream.read()
            with self.lock:
                self.ok = ok
                self.frame = frame
                self.seen_latest = False
                # if (fc := frame.copy()) is not None:
                    # self.image_pool[self.image_pool_next] = fc
                    # self.image_pool_next += 1
                    # self.image_pool_next %= POOL_SIZE

    def read(self):
        with self.lock:
            self.seen_latest = True
            frame = self.frame.copy()
            return frame

    def has_new_data(self):
        return not self.seen_latest

    def is_stopped(self):
        return self.stopped

    def stop(self):
        if self.stopped:
            return
        with self.lock:
            self.stopped = True
        self.stream.release()
        # save captured data
        # for inc in range(POOL_SIZE):
            # i = (self.image_pool_next + inc) % POOL_SIZE
            # if (img := self.image_pool[i]) is not None:
            #     fname = f"images/{self.name}_{inc}.jpg"
            #     cv2.imwrite(fname, img)

class Controller(ABC):
    @abstractmethod
    def read(self) -> str: ...

    @abstractmethod
    def send(self, s: str): ...


class DebugController(Controller):
    def read(self):
        return input(
            "[ debug controller ] Please enter message to send through controller: "
        )

    def send(self, s):
        print(f"[ debug controller ] > {s}")


class SocketController(Controller):
    def __init__(self, ip, port):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((ip, port))
        self.sock.setblocking(False)

    def read(self):
        """Returns one line receieved from the controller"""
        data = b""
        while True:
            chunk = self.sock.recv(1)
            if not chunk:
                break
            if chunk == b"\n":
                break
            data += chunk
        return data.decode().strip()

    def send(self, s):
        self.sock.sendall((s + "\n").encode())


class MultiController(Controller):
    def __init__(self, read_controller, controllers):
        self.controllers = [read_controller] + controllers
        self.read_controller = read_controller

    def read(self):
        return self.read_controller.read()

    def send(self, s):
        for c in self.controllers:
            c.send(s)
