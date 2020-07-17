# Work

## Compile
- Use the following command line to copy and compile files in folder [model](../model/):
```console
user@pc:~covid-abms/hpc_repast/work$ make rhpc
```

## Run
- You can run the model in the command line. For instance:
```console
user@pc:~covid-abms/hpc_repast/work$ mpirun -n 4 ./bin/code_01.exe ./props/config.props ./props/model.props stop.at=2 count.of.agents=1 buffer.size=1e-5 procs.x=2 procs.y=2 origin.x=-100 origin.y=-100 extent.x=200 extent.y=200 random.seed=1
```
- You can also run the model with different properties using files. For instance:
```console
user@pc:~covid-abms/hpc_repast/work$ chmod +x run.txt
user@pc:~covid-abms/hpc_repast/work$ ./run.txt
```
