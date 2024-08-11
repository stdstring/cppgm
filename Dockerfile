FROM ubuntu:24.04
#ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt install -y \
    build-essential \
    gdb \
    cmake

#ENTRYPOINT ["/bin/bash"]