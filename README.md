# Nrf52840-usb

docker build -t nrfconnect-sdk --build-arg sdk_nrf_version=v2.6-branch .

docker run -it --privileged --device=/dev/ttyACM0 -v ${PWD}:/workdir/project nrfconnect-sdk /bin/bash

