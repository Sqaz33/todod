pipeline {
    agent {
        docker {
            image 'docker:20.10.16'
            args '-v /var/run/docker.sock:/var/run/docker.sock'
        }
    }
    stages {
        stage('Build') {
            steps {
                sh 'docker build -t todod:0.2 .'
            }
        }
        stage('Deploy') {
            steps {
                sh '''
                docker rm -f todod || true
                docker run -d -p 6666:6666 -v todod_data:/data --name todod todod:0.2
                '''
            }
        }
    }
}