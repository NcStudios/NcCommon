FROM ubuntu:22.04

RUN apt update && \
    apt -y install git g++-12 make cmake && \
    ln -s /usr/bin/g++-12 /usr/bin/g++