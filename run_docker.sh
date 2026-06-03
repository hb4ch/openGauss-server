#!/bin/bash
docker build -t opengauss-ubuntu .
docker run --rm -e GS_PASSWORD=Enmo@123 -p 5432:5432 opengauss-ubuntu
