: /bin/sh
#RUN CONQUEST WHEN AT COMMAND DOES NOT EXIST
#date 
#while [ 1 ] 
#do  
#$(EXECUTABLE)/conquest -x 
#/bin/sleep 86400 
#done 

#please read up on the at command,  this now, and 24 hours from the time run.
date 
conquest -x
at now+1day << 'EOF' 
	run
EOF
