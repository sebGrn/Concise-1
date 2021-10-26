docker volume create concise_vol
docker run -it -v concise_vol:/usr/src/app/ concise:latest

#python3.8 ./concise.py --inputFile=Benchmark/ac_90k.dat --mandatory=False --measures=False --threshold=1.0 --binary=False