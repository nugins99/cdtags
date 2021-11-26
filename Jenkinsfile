pipeline {
  agent {
    docker {
      image 'centos:8'
    }

  }
  stages {
    stage('error') {
      steps {
        sh 'yum install -y cmake gcc'
      }
    }

  }
}