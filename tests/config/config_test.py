import subprocess

BLUE = '\033[34m'
RED = '\033[31m'
GREEN = '\033[32m'
RESET = '\033[0m'

print("[CONFIG FILE TESTS]\n")
def confTest(confPath: str, expectedCode: int):
    webserv = subprocess.Popen(["./webserv", confPath], 
        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    print(f"{BLUE}Testing config file: {confPath}")
    exitCode: int = 0
    try:
        exitCode = webserv.wait(0.1)
    except:
        webserv.terminate()
    if exitCode == expectedCode:
        print(f"{GREEN}OK{RESET}")
    else:
        print(f"{RED}KO{RESET}")

tests = {
        "./tests/config/empty.conf": 1,
        "./config/nginx.conf": 0,
        "./tests/config/empty_server.conf": 1,
        "./tests/config/invalid_ip.conf": 1,
        "./tests/config/server_in_server.conf": 1,
        }

for key in tests:
    confTest(key, tests[key])
