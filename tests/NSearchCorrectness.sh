#!/usr/bin/env bash

for ((i = 0 ; i < 100; i++)); do
  ../build/bin/NSearchCorrectness > /dev/null
  if [[ $? -ne 0 ]]; then
    echo Test NSearchCorrectness failed
    exit -1
  fi
done