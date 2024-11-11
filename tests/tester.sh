RED='\e[31m'
GREEN='\e[32m'
YELLOW='\e[33m'
BLUE='\e[34m'
MAGENTA='\e[35m'
CYAN='\e[36m'
RESET='\e[0m'

./webserv ./config/test.conf &
PID=$!

sleep 1
code=$(curl -s -o /dev/null -w "%{http_code}" http://localhost:3434)
if $code == 404:
	echo "SUCCESS"

echo -e "${RED}webserv shutting down"
kill $PID
