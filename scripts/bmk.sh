#!/bin/bash

scripts/release.sh

rsync -avz -e ssh release/ enes@192.168.1.200:/home/enes/release
