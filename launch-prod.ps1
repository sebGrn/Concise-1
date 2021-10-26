#docker volume create --name qc_vol --opt type=none --opt device=$(pwd) --opt o=bind
#docker run -it --mount source=qc_vol,target=/usr/src/app concise:latest

#docker run -it --name concise_vol -v "d/dev/Concise:/usr/src/app/" concise:latest

docker volume create concise_vol
docker run -it -v concise_vol:/usr/src/app/ concise:latest

#python3.8 ./concise.py --inputFile=Benchmark/ac_90k.dat --mandatory=False --measures=False --threshold=1.0 --binary=False