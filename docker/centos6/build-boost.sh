cd /build
tar -xzvf boost_1_75_0.tar.gz
cd /build/boost_1_75_0
./bootstrap.sh --with-libraries=program_options,filesystem --prefix=/opt/boost
./b2 --prefix=/opt/boost --layout=versioned link=static
./b2 install
