#!/bin/sh

# simple build docker image

docker container rm pico-sdk -f
#docker build . --tag raspberry-pi-pico-docker-dev:$(date +%s)
docker build . --tag raspberry-pi-pico-docker-dev:latest
