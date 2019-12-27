outFile="res.txt"

echo "1/4 Rebuilding"
cd ..
make rebuild

echo "2/4 Copying"
cp sopang end_to_end_tests/sopang
cd end_to_end_tests

echo "3/4 Running"

# Exact
./sopang text_test.eds patterns_test.txt > $outFile
python3 check_result.py "2 1 1 1 1 2 1 1"

# Approx
./sopang text_test.eds patterns_test.txt -k 1 > $outFile
python3 check_result.py "2 3 3 3 3 3 1 1"

# With sources
./sopang text_test.eds patterns_test.txt -S sources_test.edss > $outFile
python3 check_result.py "2 1 1 1 1 2 0 1"

./sopang text_test.eds patterns_test.txt -S sources_test.edss --full-sources-output > $outFile
python3 check_result.py "2 1 1 1 1 2 0 1"

echo "4/4 Teardown"
rm -f sopang $outFile
