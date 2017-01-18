FROM centos:6
RUN yum -y update
RUN yum install -y centos-release-scl-rh 
RUN yum install -y centos-release-scl
RUN yum install -y devtoolset-4-binutils 
RUN yum install -y devtoolset-4-gdb 
RUN yum install -y devtoolset-4-gcc-c++
