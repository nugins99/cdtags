FROM centos6-base

RUN rm  /etc/yum.repos.d/*
COPY CentOS-Base.repo /etc/yum.repos.d
RUN yum clean all
RUN yum update -y
RUN yum install -y build-essential cmake boost-devel vim
RUN yum -y groupinstall "Development tools"
RUN yum -y install centos-release-scl
COPY CentOS-SCLo-scl.repo /etc/yum.repos.d
COPY CentOS-SCLo-scl-rh.repo /etc/yum.repos.d
RUN yum -y install rh-mariadb102-boost-devel rh-mariadb102-boost-static  devtoolset-8
RUN yum  -y install openssh-server sudo

RUN ssh-keygen -t dsa -P ""  -f /etc/ssh/ssh_host_dsa_key
RUN ssh-keygen -t rsa -P ""  -f /etc/ssh/ssh_host_rsa_key
RUN ssh-keygen -t ecdsa -P ""  -f /etc/ssh/ssh_host_ecdsa_key
#RUN ssh-keygen -t ed25519 -P ""  -f /etc/ssh/ssh_host_ed25519_key
#RUN ssh-keygen -t ed25519-sk -P ""  -f /etc/ssh/ssh_host_dsa_key

RUN ( \
    echo 'LogLevel DEBUG2'; \
    echo 'PermitRootLogin yes'; \
    echo 'PasswordAuthentication yes'; \
    echo 'Subsystem sftp /usr/libexec/openssh/sftp-server'; \
  ) > /etc/ssh/sshd_config_test_clion

RUN useradd -m build \
  && yes d6d3b07b4de0a8e64b578b1c3daa58d8 | passwd build

RUN useradd -m admin \
  && yes d6d3b07b4de0a8e64b578b1c3daa58d8 | passwd admin
RUN usermod admin -G wheel

COPY id_rsa.pub /home/build/.ssh/authorized_keys
COPY id_rsa.pub /home/admin/.ssh/authorized_keys

RUN chmod 0600 /home/build/.ssh/authorized_keys
RUN chmod 0600 /home/admin/.ssh/authorized_keys

RUN chown -R build.build  /home/build/.ssh 
RUN chown -R admin.admin  /home/admin/.ssh 

copy devenv.sh /etc/profile.d/devenv.sh
copy boost_1_75_0.tar.gz /build/boost_1_75_0.tar.gz 
copy build-boost.sh /build 
RUN chmod +x /build/build-boost.sh
RUN  /usr/bin/scl enable devtoolset-8 /build/build-boost.sh

CMD ["/usr/sbin/sshd", "-D", "-e", "-f", "/etc/ssh/sshd_config_test_clion"]
#CMD ["/bin/bash"]
