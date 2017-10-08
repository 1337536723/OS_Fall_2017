#Some thoughts and useful tips for HW1

## How to use shared memory
* [Here](http://blog.csdn.net/guoping16/article/details/6584058)
## How to trace process?
```
$ strace -f -tt -e trace=execve ./execution_file
```
## How to check zombie process?
```
$ ps aux | grep 'Z'
```
* where you can see the zomvie process's PID and cpu usage
  or use the following
```
$ ps aux | grep "defunt"
```
* displays the defunct process

* and use the following to trace its parent_pid
```
$ pstree -p -s zombie_pid
```

The follwing pics will show.

* Usually, the parent will execute ahead before the child and wait it,
so if the child execute beforehand, a timing error will be generated.
* Reference to :  [Here](http://www.dropwizard.io/1.0.2/docs/) - zh-tw

## Result of this HW  
* Problems with data racing
![alt text]()
* Problem 1 result, check all of the process terminated successfully and no zombie process remains
  (by using grep 'Z')
![alt text]()
* Problem 1 result, see the process has been accelerated
  (by using strace while the process is executing and all the children terminated successfully)
![alt text]()  

* Cool discoveries
  1.Without gcc optimization o3
![alt text]()
  2.With gcc optimization
![alt text]()
