#!/bin/sh

#docker rm $(docker ps --filter status=exited -q)
docker container rm pico-sdk -f
docker build . --tag raspberry-pi-pico-docker-dev:$(date +%s)
docker run -d -it -u $(id -u):$(id -g) --name pico-sdk --mount type=bind,source=${PWD},target=/home/dev/src raspberry-pi-pico-docker-dev:latest
docker exec -it pico-sdk /bin/sh