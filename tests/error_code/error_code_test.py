import signal
from time import sleep
import requests
import subprocess
import sys

BLUE = '\033[34m'
RED = '\033[31m'
GREEN = '\033[32m'
RESET = '\033[0m'

webserv = subprocess.Popen(["./webserv", "./tests/error_code/error_code.conf"],
    stdout=subprocess.DEVNULL)
sleep(0.5)
def shutdown(signal, frame):
    print(f"${RED}Shutting down webserv...${RESET}")
    webserv.terminate()
    sys.exit(0)

signal.signal(signal.SIGINT, shutdown)

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
        print(f"{GREEN}OK: {response.status_code}{RESET}")
    else:
        print(f"{RED}KO: {response.status_code} instead of {exceptedCode}{RESET}")

tests = {
        "http://localhost:3434": 404,
        "http://localhost:3434/list": 200,
        "http://localhost:3434/broken.php": 500,
        "http://localhost:3434/test.php": 200,
        "http://localhost:3434/test.php/": 200,
        "http://localhost:3434/upload": 200,
        "http://localhost:3434/upload/": 200,
        "http://localhost:3434/oui": 405,
        "http://localhost:3434/oui/": 405,
        "http://localhost:3434/timeout.py/": 504,
        }

print("[ERROR CODE TESTS]")
print("\nTests without hostname:")
for key in tests:
    testGet(tests[key], key)


print("\nTests with hostname test.com:")
for key in tests:
    testGet(tests[key], key, "test.com")

print("\nTests with invalid hostname:")
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
        "http://localhost:3434/oui": 405,
        "http://localhost:3434/oui/": 405,
        "http://localhost:3434/timeout.py/": 404,
        }

print("\nTests with hostname oui.com:")
for key in tests:
    testGet(tests[key], key, "oui.com")

tests = {
        "http://localhost:3434": 500,
        "http://localhost:3434/": 500,
        "http://localhost:3434/POST.html": 200,
        "http://localhost:3434/list": 404,
        "http://localhost:3434/broken.php": 500,
        "http://localhost:3434/test.php": 500,
        "http://localhost:3434/test.php/": 500,
        "http://localhost:3434/upload": 404,
        "http://localhost:3434/upload/": 404,
        "http://localhost:3434/oui": 405,
        "http://localhost:3434/oui/": 405,
        "http://localhost:3434/timeout.py/": 500,
        }

print("\nTests with hostname invalidCGI.com:")
for key in tests:
    testGet(tests[key], key, "invalidCGI.com")

webserv.terminate()
