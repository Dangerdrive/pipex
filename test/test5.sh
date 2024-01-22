#!/bin/bash

#Test files
INPUT=test/envp.txt
INPUT_INVALID=test/none.what
INPUT_INFINITE=/dev/urandom
OUTPUT_EXPECTED=test/expected_output.txt
OUTPUT_PIPEX=test/output.txt
OUTPUT_INVALID=test/forbidden.txt

#Test shortcuts
COMMAND_WITH_PATH=/usr/bin/cat
LEAK_TOGGLE=1           #Set to 1 to run Valgrind, some other value for no check
VALGRIND="valgrind --leak-check=full --show-leak-kinds=all"

#Colors
NC="\033[0m"
CYAN="\033[36m"
BOLD="\033[1m"
ULINE="\033[4m"
RED="\033[31m"
GREEN="\033[32m"
YELLOW="\033[33m"
BLUE="\033[34m"
PURPLE="\033[35m"

cd ..
printf "${YELLOW}${BOLD}\n=========== MAKEFILE =============\n${NC}${PURPLE}"
make re
#printf "${YELLOW}${BOLD}\n========== NORMINETTE ============\n${NC}${CYAN}"
#norminette

printf "${PURPLE}${BOLD}\n==================================\n"
printf                   "|           MANDATORY            |"
printf                  "\n==================================\n${NC}"

printf "${YELLOW}${BOLD}\n============ TEST 5 ==============\n${NC}"
rm -f $OUTPUT_EXPECTED $OUTPUT_PIPEX
touch $OUTPUT_EXPECTED $OUTPUT_PIPEX
printf "Input file does not exist. Error message should contain \"No such file or directory\"\n"
printf "Shell command: ${BOLD}${BLUE}<$INPUT_INVALID cat | wc -l > $OUTPUT_EXPECTED${NC}${YELLOW}\n"
< $INPUT_INVALID cat | wc -l > $OUTPUT_EXPECTED
printf "${NC}Pipex command: ${BOLD}${BLUE}./pipex $INPUT_INVALID \"cat\" \"wc -l\" $OUTPUT_PIPEX${NC}\n${YELLOW}"
./pipex $INPUT_INVALID "cat" "wc -l" $OUTPUT_PIPEX
printf "${NC}Output file: "
if cmp -s $OUTPUT_EXPECTED $OUTPUT_PIPEX; then
    printf "${GREEN}${BOLD}OK!${NC}\n"
else
    printf "${RED}${BOLD}KO: Output differs${NC}:\n"
    diff --color -c $OUTPUT_EXPECTED $OUTPUT_PIPEX
fi
if [ $LEAK_TOGGLE -eq 1 ]; then
    printf "Leak check:${CYAN}\n"
    $VALGRIND ./pipex $INPUT_INVALID "cat" "wc -l" $OUTPUT_PIPEX
fi
