# syntax=docker/dockerfile:1
FROM ubuntu:20.04
RUN /bin/sh -c 'apt update && apt install vim gcc-10 g++-10 -y\
&& update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 90 --slave /usr/bin/g++ g++ /usr/bin/g++-10 --slave /usr/bin/gcov gcov /usr/bin/gcov-10'
WORKDIR /root
COPY . .
CMD ["gcc, "-v"]
