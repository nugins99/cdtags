build: boost_1_75_0.tar.gz
	cp ~/.ssh/id_rsa.pub . 
	docker build --network host -t centos6-build .

boost_1_75_0.tar.gz: 
	wget https://dl.bintray.com/boostorg/release/1.75.0/source/boost_1_75_0.tar.gz

run:
	docker run --hostname centos6-build  -d --cap-add sys_ptrace\
	 	-p127.0.0.1:2222:22 -t centos6-build
	
attach: 
	docker run --hostname centos6-build  --network host -it centos6-build
