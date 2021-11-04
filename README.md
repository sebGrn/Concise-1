# Concise

## With Docker 

### Build container
```
docker build -t concise .
```

### Run the container
```
docker volume create concise_vol
docker run -it -v concise_vol:/usr/src/app/ concise:latest
```

## Without Docker


| Dependancies | version  |
| ------ | ------ |
| Python | 3.8 |
| g++ | 10 |
| CMake | 3.17.2 |


 - Python installation
 ```
 apt-get install python3.8
 ```
 
 - g++ installation
```
apt-get install g++
```

 - cmake installation
Install build tools and libraries that CMake depends on:
```
apt-get install build-essential libssl-dev
```
download and extract the source code
```
wget https://github.com/Kitware/CMake/releases/download/v3.20.0/cmake-3.20.0.tar.gz
tar -zxvf cmake-3.20.0.tar.gz
```
compile and install CMake
```
cd cmake-3.20.0
./bootstrap
```

## Launch Consice 
```
python3.8 ./concise_benchmark.py
```
```
python3.8 ./concise.py --inputFile=Benchmark/ac_90k.dat --mandatory=False --measures=False --threshold=1.0 --binary=False
```
