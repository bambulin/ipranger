#!/bin/bash

pushd /opt/iprangerzip/

if [[ $MODE == "READING" ]]; then
    while [ 1 ]; do ./bin/iprangercli ./testing/test.txt -t; done
else if [[ $MODE == "WRITING" ]]; then
        while [ 1 ]; do ./bin/iprangercli ./testing/config.txt -c; done
    else
        echo "Unexpected mode. Check MODE env prop."
        exit 1
    fi
fi
