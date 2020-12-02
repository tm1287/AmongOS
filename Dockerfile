FROM ubuntu:latest as build
RUN apt-get update \
    && apt install members -y
   # && apt install openssh-server -y

COPY bashrc_additions /root/bashrc_additions
RUN cat /root/bashrc_additions >> /root/.bashrc
RUN rm /root/bashrc_additions

RUN for group in crewmates imposters players players-host;do groupadd ${group};done

#ENTRYPOINT service ssh restart && bash