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
                   mkdir build ;
                   cd build ;
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
                   mkdir build ;
                   cd build ;
                   export BOOST_DIR=/opt/boost
                   /usr/bin/scl enable devtoolset-8 '/build/cmake-3.22.0-linux-x86_64/bin/cmake -DSTATIC=1 -DCMAKE_BUILD_TYPE=Release ..' ;
                   /usr/bin/scl enable devtoolset-8 'make -j4'; 
                   /usr/bin/scl enable devtoolset-8 'make package';
                   '''
            }
        }
    }
}
