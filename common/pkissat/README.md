### Compile
```
make clean; make
```


### Run

$instance: input cnf

$threads: number of threads

$time-limit: maximum running time

$shr: 1 => one sharing group, 2 => two sharing group

-horde: dynamic clause filtering

-str: one thread in charge of strengthening learnt clauses per sharing group

-dup: remove/promote duplicate clauses


```
time ./painless-mcomsps -c=(($threads-1)) -shr-sleep=500000 -shr-lit=1500 $instance -t=$time-limit -initshuffle -shr-strat=$shr [-horde] [-str] [-dup]
```






