FROM ubuntu:24.04
#ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt install -y build-essential && apt install -y gdb

#ENTRYPOINT ["/bin/bash"]