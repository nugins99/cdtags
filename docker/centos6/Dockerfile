FROM centos:6

RUN rm  /etc/yum.repos.d/*
COPY CentOS-Base.repo /etc/yum.repos.d
COPY CentOS-SCLo-scl.repo /etc/yum.repos.d
COPY CentOS-SCLo-scl-rh.repo /etc/yum.repos.d
RUN yum clean all
RUN yum update -y
RUN yum -y groupinstall "Development tools"
RUN yum install -y build-essential cmake boost-devel vim centos-release-scl wget devtoolset-8 rh-python36-python-pip
RUN scl enable rh-python36 devtoolset-8 "pip install conan"

RUN mkdir /build
# Build Env
COPY devenv.sh /etc/profile.d/devenv.sh

# Need CMake
RUN wget https://github.com/Kitware/CMake/releases/download/v3.22.0/cmake-3.22.0-linux-x86_64.tar.gz
RUN cp cmake-3.22.0-linux-x86_64.tar.gz /build/cmake-3.20.0-linux-x86_64.tar.gz
RUN tar -C /build  -xzvf /build/cmake-3.20.0-linux-x86_64.tar.gz

# Build boost
#RUN wget https://boostorg.jfrog.io/artifactory/main/release/1.75.0/source/boost_1_75_0.tar.gz
#RUN cp boost_1_75_0.tar.gz /build/boost_1_75_0.tar.gz 
#COPY build-boost.sh /build 
#RUN chmod +x /build/build-boost.sh
#RUN /usr/bin/scl enable devtoolset-8 /build/build-boost.sh


