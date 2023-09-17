#!/bin/sh
containerName="pico-sdk"

# Check if container exists and is exited, if yes remove it
if [ "$(docker container ls -a -f "name=$containerName" -f "status=exited" | grep -c $containerName)" -gt 0 ]; then
    echo "Container $containerName exists and is exited"
    docker container rm $containerName -f
fi

docker run -d -w /home/dev/src -it -u $(id -u):$(id -g) --name pico-sdk --mount type=bind,source=${PWD},target=/home/dev/src raspberry-pi-pico-docker-dev
#docker run -d -w /home/dev/src -it --name pico-sdk --mount type=bind,source=${PWD},target=/home/dev/src raspberry-pi-pico-docker-dev

docker exec -it pico-sdk $1