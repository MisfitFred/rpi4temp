#!/bin/sh

containerName="pico-sdk"

#docker rm $(docker ps --filter status=exited -q)
docker container rm $containerName -f
containerDate=$(date +%s)

# Check if container exists and is exited and remove it
if [ "$(docker container ls -a -f "name=$containerName" -f "status=exited" | grep -c $containerName)" -gt 0 ]; then
    echo "Container $containerName exists and is exited"
    docker container rm $containerName -f
fi

docker build . --tag raspberry-pi-pico-docker-dev:containerDate
docker run -d -it -u $(id -u):$(id -g) --name $containerName --mount type=bind,source=${PWD},target=/home/dev/src raspberry-pi-pico-docker-dev:containerDate
docker exec -it $containerName /bin/sh