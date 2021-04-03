FROM ubuntu:20.04

RUN apt update

RUN apt-get install -y build-essential zlib1g-dev libncurses5-dev libgdbm-dev libnss3-dev libssl-dev libreadline-dev libffi-dev libsqlite3-dev wget libbz2-dev

RUN wget https://www.python.org/ftp/python/3.8.0/Python-3.8.0.tgz && tar -xf Python-3.8.0.tgz && cd Python-3.8.0 && ./configure --enable-optimizations && make -j 8 && make altinstall

RUN apt-get update && apt-get install -y libomp-dev && apt install -y g++-10 && update-alternatives --install  /usr/bin/g++ g++ /usr/bin/g++-10 1 

RUN wget https://github.com/Kitware/CMake/releases/download/v3.17.2/cmake-3.17.2-Linux-x86_64.sh \
      -q -O /tmp/cmake-install.sh \
      && chmod u+x /tmp/cmake-install.sh \
      && mkdir /usr/bin/cmake \
      && /tmp/cmake-install.sh --skip-license --prefix=/usr/bin/cmake \
      && rm /tmp/cmake-install.sh

ENV PATH="/usr/bin/cmake/bin:${PATH}"

RUN mkdir -p /usr/src/app

WORKDIR /usr/src/app

COPY . /usr/src/app

EXPOSE 8080

RUN alias python3=python3.8