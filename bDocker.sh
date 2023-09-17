#!/bin/sh

# simple build docker image

containerName="pico-sdk"

docker container rm $(containerName) -f

# Check if container exists and is exited and remove it
if [ "$(docker image ls $containerName | grep -c $containerName)" -gt 0 ]; then
    echo "Container $containerName exists and is exited"
    docker image rm $containerName -f
fi

docker build . --tag raspberry-pi-pico-docker-dev:$(date +%s)
#docker build . --tag raspberry-pi-pico-docker-dev:latest
