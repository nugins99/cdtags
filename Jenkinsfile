pipeline {
    agent none
    
    stages {
        stage('CentOS8') {
            agent {
                    dockerfile {
                        filename 'docker/centos8/Dockerfile'
                    }
            }
            steps {
                sh '''
                   mkdir build ;
                   cd build ;
                   cmake -DCMAKE_BUILD_TYPE=Release .. ;
                   make ; 
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
                   /opt/cmake/cmake-3.22.0-linux-x86_64/bin/cmake -DSTATIC=1 -DCMAKE_BUILD_TYPE=Release .. ;
                   make ; 
                   make package ;
                   '''
            }
        }
    }
}
