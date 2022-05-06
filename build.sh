#!/bin/sh

set -x

if [ ! -d "./Build" ]; then
  mkdir ./Build 
fi

if [ ! -d "./LogFiles" ]; then
  mkdir ./LogFiles
fi

cd ./Build \
  && cmake .. \
  && make
