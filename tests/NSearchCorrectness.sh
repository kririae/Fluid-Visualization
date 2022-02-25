#!/usr/bin/env bash

N=500
for ((i = 0 ; i < $N; i++)); do
  ../build/bin/NSearchCorrectness > /dev/null
  if [[ $? -ne 0 ]]; then
    echo Test NSearchCorrectness failed
    exit -1
  fi
done
echo "$N times test passed!"