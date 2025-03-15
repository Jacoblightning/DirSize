#!/usr/bin/env bash

set -vx

cd "$(mktemp -d)" || exit 1

echo 5char > file.txt

output=$($1 -m .)

echo "Got Output: $output"

test "${output:2}" -eq 6