#!/bin/bash

containerName="pico-sdk"

# Check if container exists and is exited, if yes remove it
    if [ "$(docker container ls -a -f "name=$containerName" -f "status=exited" | grep -c $containerName)" -gt 0 ]; then
        echo "Container $containerName exists and is exited"
        docker container rm $containerName -f
    else
        echo "Container $containerName does not exist or is not exited"
    fi


# Check if container already running, if not start it
    if [ "$(docker container ls -a -f "name=$containerName" -f "status=running" | grep -c $containerName)" -gt 0 ]; then
        echo "Container $containerName exists and is running"
    else
        echo "Container $containerName does not exist or is not running"
        docker run -d -w /home/dev/src -it -u $(id -u):$(id -g) --name pico-sdk --mount type=bind,source=${PWD},target=/home/dev/src fred78/raspberry-pi-pico-sdk:latest
    fi


docker exec -it pico-sdk $1