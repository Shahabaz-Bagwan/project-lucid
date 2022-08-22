#!/bin/bash

# color ref https://stackoverflow.com/questions/5947742/how-to-change-the-output-color-of-echo-in-linux
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${RED}Run this only from docker folder${NC}"

echo "building docker image.... "
docker build -t project-lucid:1.0 .

echo "running the container.."
dir=$(dirname `pwd`)
echo $dir
docker run -d -it --name project-lucid --mount type=bind,source=$dir,target=/projects project-lucid:1.0 && check=1
