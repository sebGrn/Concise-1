# Concise

## Build container
```
docker build -t concise .
```

## Run the container
```
docker volume create concise_vol
docker run -it -v concise_vol:/usr/src/app/ concise:latest
```

## Run Concise inside the container
```
python3.8 ./concise_benchmark.py
```
```
python3.8 ./concise.py --inputFile=Benchmark/ac_90k.dat --mandatory=False --measures=False --threshold=1.0 --binary=False
```
