outFile="res.txt"

echo "1/4 Rebuilding"
cd ..
make rebuild

echo "2/4 Copying"
cp sopang end_to_end_tests/sopang
cd end_to_end_tests

echo "3/4 Running"

./sopang text_test.eds patterns_test.txt > $outFile
python check_result.py "2 1 1 1 1 2"

echo "4/4 Teardown"
rm -f sopang $outFile
