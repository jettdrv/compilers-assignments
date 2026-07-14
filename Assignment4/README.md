Per eseguire il pass:
```
opt -load-pass-plugin ./libLoopFusion.so -passes=loop-fusion-pass ../test/fusion.ll -So ../test/fusionOpt.ll
```