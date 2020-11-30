FROM ubuntu:latest as build
RUN apt-get update

COPY bashrc_additions /root/bashrc_additions
RUN cat /root/bashrc_additions >> /root/.bashrc
RUN rm /root/bashrc_additions