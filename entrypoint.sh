#!/bin/bash
set -e

# Define help message
show_help() {
  echo """
  Commands
  ---------------------------------------------------------
  bash          : run bash
  eval          : eval shell command
  build         : build the app [arg: path to cmake folder] 
  run           : run the application [arg: path to program] 
  test          : test the application  
  """
}

build(){
    cd "./build"
    cmake ..
    cmake --build .
}

case "$1" in

  bash )
    bash
  ;;

  eval )
    eval "${@:2}"
  ;;

  build )
    build
  ;;

  test )
    eval "./build/tests/tests ${@:2}"
  ;;

  run )
    eval "./build/main/app ${@:2}"
  ;;

  ftest )
    build && eval "./tests/tests ${@:2}"
  ;;

  frun )
    build && eval "./main/app ${@:2}"
  ;;

  * )
    show_help
  ;;

esac
