node('linux') {
	stage 'checkout'
	deleteDir()
	checkout scm
	sh 'git submodule init'

	withCredentials([usernameColonPassword(credentialsId: '5bcf8245-68c4-43e8-a150-5d037909863f', variable: 'CREDENTIALS')]) {		
		sh 'git config --file=.gitmodules submodule.AVR-HAL.url "https://${CREDENTIALS}@github.com/PW-Sat2/AVR-HAL.git"'
	}
	sh 'git submodule sync'
	sh 'git submodule update'
	
	stage 'clean'
	sh 'make clean'

	stage 'checkStyle'
	sh 'make checkStyle'
	
	stage 'checkFormat'
	sh 'make checkFormat'

	stage 'PLD.build'
	sh 'make PLD_build'

	stage 'PLD.unit_tests'
	sh 'make PLD_unit_tests'
}
