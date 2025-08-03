#!/bin/bash

## TODO: space problems

RTW_EXEC="../../bin/rtw/rtw"
TEST_DIR="./cases"

mkdir -p "$TEST_DIR"
echo -n "" > "$TEST_DIR/empty.txt"

$RTW_EXEC "$TEST_DIR/empty.txt" > "$TEST_DIR/out.txt"

if ! grep -q "bytes:0  chars:0  newlines:0  words:0    :"$TEST_DIR"/empty.txt" "$TEST_DIR/out.txt"; then
  echo "Test empty file failed"
  cat "$TEST_DIR"/out.txt
  exit 1
fi

echo -e "Hello world\nThis is a test." > "$TEST_DIR"/simple.txt
$RTW_EXEC "$TEST_DIR/simple.txt" > "$TEST_DIR/out.txt"

if ! grep -q "bytes:28  chars:28  newlines:2  words:6    :"$TEST_DIR"/simple.txt" "$TEST_DIR/out.txt"; then
  echo "Test simple file failed"
  cat "$TEST_DIR"/out.txt
  exit 1
fi

echo -e "word1    word2\t\tword3\n\n" > "$TEST_DIR"/spaces.txt
$RTW_EXEC "$TEST_DIR/spaces.txt" > "$TEST_DIR/out.txt"

if ! grep -q "bytes:24  chars:24  newlines:3  words:3    :"$TEST_DIR"/spaces.txt" "$TEST_DIR/out.txt"; then
  echo "Test spaces file failed"
  cat "$TEST_DIR"/out.txt
  exit 1
fi

echo "All tests passed!"
