FROM centos:8
RUN dnf groupinstall -y "Development Tools"
RUN yum  install -y cmake  boost-devel python3-pip
RUN pip3 install conan
RUN mkdir /.conan
