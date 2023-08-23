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


