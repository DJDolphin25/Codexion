timeout 20s valgrind --tool=helgrind --trace-children=yes \
  --log-file=helgrind-codexion.log ./codexion 5 800 200 200 200 2 1000 fifo
```