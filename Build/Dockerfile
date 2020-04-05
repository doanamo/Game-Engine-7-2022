# Making changes to this file requires triggering image rebuild on Docker Hub!

FROM amd64/ubuntu:bionic

RUN dpkg --add-architecture i386

RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | sudo apt-key add -
RUN apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main'

RUN apt-get update
RUN apt-get install -y git g++-multilib cmake
RUN apt-get install -y libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev
RUN apt-get install -y libgl1-mesa-dev libglu1-mesa-dev
RUN apt-get install -y libx11-dev:i386 libxrandr-dev:i386 libxinerama-dev:i386 libxcursor-dev:i386
RUN apt-get install -y libgl1-mesa-dev:i386 libglu1-mesa-dev:i386
