FROM gcc:7.3.0

RUN apt-get -qq update
RUN apt-get -qq upgrade

# Install CMake 3.10
RUN wget -q https://cmake.org/files/v3.12/cmake-3.12.0.tar.gz -O- \
    | tar -C /tmp -xz && cd /tmp/cmake-3.12.0/ && ./bootstrap && \
    make && make install && cd && rm -rf /tmp/cmake-3.12.0

RUN apt-get -qq install libboost-all-dev=1.62.0.1
RUN apt-get -qq install build-essential libtcmalloc-minimal4 && \
  ln -s /usr/lib/libtcmalloc_minimal.so.4 /usr/lib/libtcmalloc_minimal.so

COPY . /usr/project
WORKDIR /usr/project/build

RUN cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
RUN cmake --build .

WORKDIR /usr/project

EXPOSE 8080
ENTRYPOINT ["./entrypoint.sh"]
CMD ["bash"]