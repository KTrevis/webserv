import signal
from time import sleep
from typing import NoReturn
import subprocess
import sys

BLUE = '\033[34m'
RED = '\033[31m'
GREEN = '\033[32m'
RESET = '\033[0m'


def shutdown(signal, frame):
    print(f"${RED}Shutting down webserv...${RESET}")
    webserv.terminate()
    sys.exit(0)

signal.signal(signal.SIGINT, shutdown)

webserv = subprocess.Popen(["./webserv", "./tests/conf/error_code.conf"],
                           stdout=subprocess.DEVNULL, stderr=open("/dev/null"))
sleep(0.1)

exitCode = webserv.wait()
print(exitCode)
