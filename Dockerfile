# RBTREE dockerfile
# FROM ubuntu:latest

# # Avoids interactive prompts during package installation.
# ENV DEBIAN_FRONTEND=noninteractive

# # Install build-essential for compiling the C projects in this repository and
# # clean up apt caches to keep the image small.
# # RUN apt-get update \
# #     && apt-get install -y --no-install-recommends build-essential gdb make cmake git \
# #     && rm -rf /var/lib/apt/lists/*

# RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
#     locales tzdata build-essential sudo gdb vim curl git wget \
#     python3 cmake make gcc sudo valgrind telnet net-tools iproute2\
#     && locale-gen ko_KR.UTF-8 && update-locale LANG=ko_KR.UTF-8

# # The project will be mounted here from the host (see docker-compose.yml).
# WORKDIR /workspace

# # Default to bash for interactive sessions.
# CMD ["bash"]

# Base image: Ubuntu x86_64
# FROM --platform=linux/amd64 ubuntu:22.04

# ENV TZ=Asia/Seoul LANG=ko_KR.UTF-8 LANGUAGE=ko_KR.UTF-8

# # Install dependencies
# RUN export DEBIAN_FRONTEND=noninteractive && \
#     apt-get update \
#     # && echo "tzdata tzdata/Areas select Asia" | debconf-set-selections \
#     # && echo "tzdata tzdata/Zones/Asia select Seoul" | debconf-set-selections \
#     && apt-get install -y \
#     locales tzdata \
#     build-essential \
#     gcc \
#     gdb \
#     vim \
#     git \
#     sudo \
#     qemu-system-x86 \
#     python3 \
#     && locale-gen ko_KR.UTF-8 && update-locale LANG=ko_KR.UTF-8
# # && apt-get clean \
# # && rm -rf /var/lib/apt/lists/*

# RUN useradd -m -s /bin/bash jungle && usermod -aG sudo jungle
# RUN echo "jungle ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/jungle

# USER jungle
# WORKDIR /home/jungle

# RUN echo "source /workspaces/pintos_22.04_lab_docker/pintos/activate" >> /home/jungle/.bashrc


# Dockerfile for malloc
FROM ubuntu:latest

ENV TZ=Asia/Seoul LANG=ko_KR.UTF-8 LANGUAGE=ko_KR.UTF-8

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    locales tzdata build-essential sudo gdb vim curl git wget \
    python3 cmake make gcc sudo valgrind telnet net-tools iproute2\
    && locale-gen ko_KR.UTF-8 && update-locale LANG=ko_KR.UTF-8

RUN useradd -m -s /bin/bash jungle && usermod -aG sudo jungle
RUN echo "jungle ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/jungle
# 암호 설정
# RUN echo 'jungle:jungle' | chpasswd

USER jungle
WORKDIR /home/jungle
