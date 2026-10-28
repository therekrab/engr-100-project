import functools
import time

TIME_FILE = open("processing_timing.csv", "w")  # noqa: SIM115

def log_time(name):
    def wrapper_wrapper(func):
        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            start = time.time()
            result = func(*args, **kwargs)
            elapsed = time.time() - start
            TIME_FILE.write(f"{name}:{func.__name__},{elapsed}\n")
            return result
        return wrapper
    return wrapper_wrapper

def save_logged_time_data():
    TIME_FILE.close()
