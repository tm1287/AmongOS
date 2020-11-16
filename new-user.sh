#!/bin/bash

echo "Enter a username..."

read username

sudo useradd $username -g players

sudo passwd $username

sudo -u $username /bin/bash
