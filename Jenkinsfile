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
                   cmake -DSTATIC=1 -DCMAKE_BUILD_TYPE=Release .. ;
                   make ; 
                   make package ;
                   '''
            }
        }
    }
}
