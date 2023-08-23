#!/bin/sh

docker run -d -w /home/dev/src -it -u $(id -u):$(id -g) --name pico-sdk --mount type=bind,source=${PWD},target=/home/dev/src raspberry-pi-pico-docker-dev:latest
docker exec -it pico-sdk $1