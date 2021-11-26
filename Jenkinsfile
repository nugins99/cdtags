pipeline {
  agent {
    dockerfile {
        filename 'docker/centos8/Dockerfile'
    }

  }
  stages {
    stage('Build') {
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

  }
}
