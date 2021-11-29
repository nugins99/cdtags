pipeline {
    agent none
    
    stages {
        stage('CentOS8') {
            agent {
                    dockerfile {
                        dir 'docker/centos8'
                        filename 'Dockerfile'
                    }
            }
            steps {
                sh '''
                   echo "$USER:$UID"
                   mkdir build ;
                   cd build ;
                   conan remote add local http://conan:9300 ;
                   conan user -p jenkins  -r local jenkins ;
                   conan install .. ;
                   cmake -DCMAKE_BUILD_TYPE=Release .. ;
                   make -j4; 
                   make package ;
                   '''
            }
        }
        stage('CentOS6') {
            agent {
                    dockerfile {
                        dir 'docker/centos6'
                        filename 'Dockerfile'
                    }
            }
            steps {
                sh '''
                   echo "$USER:$UID"
                   mkdir build ;
                   cd build ;
                   export BOOST_DIR=/opt/boost
                   /usr/bin/scl enable rh-python36 devtoolset-8 'conan install .. --build=\'*\'''
                   /usr/bin/scl enable rh-python36 devtoolset-8 '/build/cmake-3.22.0-linux-x86_64/bin/cmake -DSTATIC=1 -DCMAKE_BUILD_TYPE=Release ..' ;
                   /usr/bin/scl enable rh-python36 devtoolset-8 'make VERBOSE=1'; 
                   /usr/bin/scl enable rh-python36 devtoolset-8 'make package';
                   '''
            }
        }
    }
}
