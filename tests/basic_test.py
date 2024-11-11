import json
import os
import signal
import socket
from time import sleep
import requests
import subprocess
import sys

BLUE = '\033[34m'
RED = '\033[31m'
GREEN = '\033[32m'
RESET = '\033[0m'

webserv = subprocess.Popen(["./webserv", "./config/test.conf"],
    stdout=subprocess.DEVNULL, stderr=open("/dev/null"))

def shutdown(signal, frame):
    print(f"${RED}Shutting down webserv...${RESET}")
    webserv.terminate()
    sys.exit(0)

signal.signal(signal.SIGINT, shutdown)

sleep(0.5)

def testGet(exceptedCode: int, url: str, hostname: str = ""):
    headers = {
            "host": hostname,
            }
    response = requests.get(url, headers=headers)
    print(f"{BLUE}Testing at url {url}{RESET}", end="")
    if hostname != "":
        print(f"{BLUE} with hostname {hostname}{RESET}", end = "")
    print()
    if response.status_code == exceptedCode:
        print(f"{GREEN}OK{RESET}")
    else:
        print(f"{RED}KO: {response.status_code} instead of {exceptedCode}{RESET}")
    return response.text

tests = {
        "http://localhost:3434": 404,
        "http://localhost:3434/list": 200,
        "http://localhost:3434/broken.php": 500,
        "http://localhost:3434/test.php": 200,
        "http://localhost:3434/test.php/": 200,
        "http://localhost:3434/upload": 200,
        "http://localhost:3434/upload/": 200,
        "http://localhost:3434/oui": 403,
        "http://localhost:3434/oui/": 403,
        }

for key in tests:
    testGet(tests[key], key)
for key in tests:
    testGet(tests[key], key, "test.com")
for key in tests:
    testGet(tests[key], key, "aled")

tests = {
        "http://localhost:3434": 200,
        "http://localhost:3434/": 200,
        "http://localhost:3434/list": 404,
        "http://localhost:3434/broken.php": 404,
        "http://localhost:3434/test.php": 404,
        "http://localhost:3434/test.php/": 404,
        "http://localhost:3434/upload": 404,
        "http://localhost:3434/upload/": 404,
        "http://localhost:3434/oui": 403,
        "http://localhost:3434/oui/": 403,
        }

for key in tests:
    testGet(tests[key], key, "oui.com")

webserv.terminate()
