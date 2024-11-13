import subprocess

BLUE = '\033[34m'
RED = '\033[31m'
GREEN = '\033[32m'
RESET = '\033[0m'

def confTest(confPath: str, expectedCode: int):
    webserv = subprocess.Popen(["./webserv", confPath], stdout=subprocess.DEVNULL)
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
        }

for key in tests:
    confTest(key, tests[key])
