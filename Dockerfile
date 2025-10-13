FROM ubuntu:latest

# Avoids interactive prompts during package installation.
ENV DEBIAN_FRONTEND=noninteractive

# Install build-essential for compiling the C projects in this repository and
# clean up apt caches to keep the image small.
# RUN apt-get update \
#     && apt-get install -y --no-install-recommends build-essential gdb make cmake git \
#     && rm -rf /var/lib/apt/lists/*

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    locales tzdata build-essential sudo gdb vim curl git wget \
    python3 cmake make gcc sudo valgrind telnet net-tools iproute2\
    && locale-gen ko_KR.UTF-8 && update-locale LANG=ko_KR.UTF-8

# The project will be mounted here from the host (see docker-compose.yml).
WORKDIR /workspace

# Default to bash for interactive sessions.
CMD ["bash"]
