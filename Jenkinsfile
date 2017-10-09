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

	stage 'EPS_A.build'
	sh 'make EPS_A_build'

	stage 'EPS_A.unit_tests'
	sh 'make EPS_A_unit_tests'

	stage 'EPS_B.build'
	sh 'make EPS_B_build'

	stage 'EPS_B.unit_tests'
	sh 'make EPS_B_unit_tests'

	stage 'SunS.build'
	sh 'make SunS_build'

	stage 'SunS.unit_tests'
	sh 'make SunS_unit_tests'

	stage 'EGSE'
	sh 'make EGSE'

	stage 'Archive'
	step([$class: 'ArtifactArchiver', artifacts: '*EPS*/cmake-build-release/src/*.hex', fingerprint: true])
	step([$class: 'ArtifactArchiver', artifacts: '*EPS*/cmake-build-release/src/*.eep', fingerprint: true])
	step([$class: 'ArtifactArchiver', artifacts: '*PLD*/cmake-build-release/src/*.hex', fingerprint: true])
	step([$class: 'ArtifactArchiver', artifacts: '*SunS*/cmake-build-release-*/src/*.hex', fingerprint: true])
}
