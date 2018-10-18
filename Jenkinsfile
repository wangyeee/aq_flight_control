pipeline {
    agent any

    stages {
        stage('Build') {
            steps {
                sh 'make BRD_VER=6 BRD_REV=0 clean all'
                sh 'make BRD_VER=8 BRD_REV=6 clean all'
                sh 'make BRD_VER=9 BRD_REV=1 clean all'
            }
        }
    }
}
