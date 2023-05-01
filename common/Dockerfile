################### Build Mallob
FROM satcomp-infrastructure:common
USER root
#  Install required softwares
RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt install -y cmake build-essential zlib1g-dev libopenmpi-dev wget unzip build-essential zlib1g-dev cmake python3 build-essential gfortran wget curl

# Build pkissat
ADD pkissat pkissat
WORKDIR /pkissat
RUN make -j 4