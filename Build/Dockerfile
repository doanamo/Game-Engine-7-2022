# Warning: Making changes to this file requires triggering image rebuild on Docker Hub!

# Specify base image.
FROM amd64/ubuntu:focal

# Add support for x86 compilation.
RUN dpkg --add-architecture i386

# Install core dependencies.
RUN apt-get update
RUN apt-get install -y apt-transport-https ca-certificates gnupg software-properties-common wget

# Add repository for latest CMake.
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | apt-key add -
RUN apt-add-repository 'deb https://apt.kitware.com/ubuntu/ focal main'

# Install developer dependencies.
RUN apt-get install -y git cmake g++-multilib clang
RUN apt-get install -y libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev
RUN apt-get install -y libgl1-mesa-dev libglu1-mesa-dev
RUN apt-get install -y libx11-dev:i386 libxrandr-dev:i386 libxinerama-dev:i386 libxcursor-dev:i386
RUN apt-get install -y libgl1-mesa-dev:i386 libglu1-mesa-dev:i386
