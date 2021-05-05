```
# syntax=docker/dockerfile:1
FROM ubuntu:20.04
RUN sudo apt install vim install gcc-10 g++-10 \
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 90 --slave /usr/bin/g++ g++ /usr/bin/g++-10 --slave /usr/bin/gcov gcov /usr/bin/gcov-10 \
WORKDIR /app
COPY . .
CMD ["gcc, "-v"]
```