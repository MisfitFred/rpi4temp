FROM lukstep/raspberry-pi-pico-sdk:latest

RUN apk update && \
    apk add gtest-dev \
    ninja

RUN apk add doxygen \
    plantuml \
    dia \
    openjdk8-jre \
    graphviz \
    jpeg-dev \
    zlib-dev \
    ttf-dejavu \
    freetype-dev \
    git \
    build-base \
    python3-dev \
    py-pip 

COPY requirements.txt /tmp/requirements.txt
RUN mkdir /home/dev && \
    cd /home/dev && \
    python -m venv sphinx  && \
    source sphinx/bin/activate && \
    pip install -r /tmp/requirements.txt

RUN apk upgrade

ARG FREERTOS_PATH=/usr/share/FreeRTOS
RUN git clone --depth 1 https://github.com/FreeRTOS/FreeRTOS.git ${FREERTOS_PATH} && \
    cd ${FREERTOS_PATH} && \
    git checkout -b v10.4.3 && \
    git submodule update --init --recursive --depth 1

ENV PICO_SDK_PATH=$FREERTOS_PATH


