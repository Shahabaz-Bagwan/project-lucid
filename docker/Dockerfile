FROM ubuntu:20.04
MAINTAINER Shahabaz Bagwan

RUN  apt-get update -q

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Berlin

RUN apt-get install -y ca-certificates

RUN  apt-get install -y --no-install-recommends \
      sudo \
      git \
      build-essential \
      clang \
      xterm \
      vim \
      bash-completion \
      python \
      cmake \
      clang-format
      

ARG user=user
RUN useradd -ms /bin/bash $user 
RUN echo "$user:$user" | chpasswd 
RUN adduser $user sudo


USER $user
ENV HOME /home/$user

RUN mkdir -p /home/$user/projects 

CMD /bin/bash
