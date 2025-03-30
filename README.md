
### ASSUMPTIONS:

- max path length of directory or file : 4096
- max command length or input length :1024
- if ur running multiple commands which takes more than 2 s then most recent command will displayed in the prompt
- in log the order is top to bottom the first to most recent 
- i am assuming that log.txt is created already and it only edited by the code 
- and i am uploading the txt file also

#### For Aliases:
1. I will storing the given input in log file no the aliase of it
2. I will handling single arg only i.e for hist-log it works only for hist no arguments should be given 

### For piping:
1.hop ..|wc I will not be hopping  to the parent directory as same as in my shell
### For activites :
1.Max no of activites is 4096
### For Signals:
1. During any sleep x if ctrl+D is pressed it exits the shell after its execution 
2. For ctrl+c if no foreground process is running and it is pressed then it just print "\n"
3. for ctrl+D during exiting i will not be printing anything similar to ctrl+c , ctrl+z 
### For Neonate :

1. if the -n is flag is not it will br returing an error
