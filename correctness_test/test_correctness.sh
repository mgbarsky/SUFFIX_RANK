INPUT_FOLDER=$1
SA_FOLDER=$2
gcc -o test_sa utils.c test_sa.c
cat $(find ${SA_FOLDER} -name "suffixarray*" | sort -V) > final_sa
./test_sa ${INPUT_FOLDER} final_sa
