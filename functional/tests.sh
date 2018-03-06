#!/bin/bash

set -o errexit

readonly DIR="$(realpath $(dirname ${BASH_SOURCE[0]}))"

server_pid=0

setup () {
  ${DIR}/../hstatic.out &
  server_pid=$!
}

tear_down () {
  kill $server_pid
}

test_get () {
  echo "${FUNCNAME[0]}"
  curl localhost:8080
}

main () {
  setup
  test_get
}

trap tear_down EXIT

main "$@"
