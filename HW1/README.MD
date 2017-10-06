#Some thoughts and useful tips for HW1

## How to trace process?
```
$ strace -f -tt -e trace=execve ./execution_file
```
## How to check zombie process?
```
$ ps aux | grep 'Z'
```

The follwing pics will show.

* Usually, the parent will execute ahead before the child and wait it,
so if the child execute beforehand, a timing error will be generated.
* Reference to :  [Here](http://www.dropwizard.io/1.0.2/docs/) - zh-tw
