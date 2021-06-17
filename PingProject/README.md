# B10702113 LIN XIU WEI PING PROJECT
Implementation of ping in Python 3.x using raw sockets.  

## Self Info.
>student ID: B10702113  
>class: 四電子三乙  
>name: 林修維  

## Argument
>positional arguments:  
>>  host host name, e.g. 192.168.43.1 or \<hostname\>

>optional arguments:  
>>  -h, --help >> show this help message and exit  
>>  -n COUNT >> ping count (default = 4)  
>>  -w TIMEOUT >> timeout (default = 2)  
>>  -d INTERVAL >> interval between each ping (default = 0)  
>>  -t >> ping continuously until the user press "Ctrl + C" (maximum = 10000)  
>>  -s ICMP_SIZE >> the size of ICMP packet (default = 32)  
>>  -i TTL >> time to live (default = 115)  
>>  -a >> resolve IP address to host name  
>> --show-time >> show system time  

## Note
> "-h" has the highest priority  
> "-t" priority is higher than "-n"  

## Enhancement In Future
> "-q" quiet, only print the last result  
> "-v" verbose, print all message in ICMP packet  
> "-l", print statistics in a cycle  
> "-6", IPv6 in ubuntu  
> GUI  