#!/bin/bash

echo -n "test string" > tmp.txt
../../bin/rtw/rtw -clwm tmp.txt > out.txt
if ! grep -q "bytes: 11 chars: 11 words: 2" out.txt; then
  echo "Test all flags failed"
  exit 1
fi

../../bin/rtw/rtw -w tmp.txt > out.txt
if ! grep -q "words: 2" out.txt; then
  echo "Test one flag failed"
  exit 1
fi

# Invalid flag test
# Stdin test

echo "All tests passed!"
