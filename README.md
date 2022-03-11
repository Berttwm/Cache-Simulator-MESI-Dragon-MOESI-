# cs4223_simulator

Compile the main code first
```
make
```

Try some default inputs
```
./coherence MESI bodytrack 4096 2 32
./coherence MOESI bodytrack 4096 2 32
./coherence Dragon bodytrack 4096 2 32

```

The Cache Simulator project, completed entirely using inbuilt C++11 libraries, is designed to test the effectiveness of different cache protocols (namely MESI, MOESI, and Dragon) with User configurable settings (Cache Size, Cache Associativity, and Cache Block size) with different large instructional data sets. As an outcome, our project also provides analytics like (total cycles, compute cycles, memory instructions, idle cycles, cache miss/hit rates, and cache utilization rates). 
