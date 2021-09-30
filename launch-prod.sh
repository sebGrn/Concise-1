docker volume create --name qc_vol --opt type=none --opt device=$(pwd) --opt o=bind
docker run -it --mount source=qc_vol,target=/usr/src/app concise:latest