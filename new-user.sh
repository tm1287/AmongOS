#!/bin/bash

players="$(members players)"

usernamegood=false

while [ "$usernamegood" = false ]
do
    echo "Enter a username..."

    read username

    if grep -qow "$username\b" <<<"$players"; then echo "Error: User already exists"; else usernamegood=true; fi
done

sudo useradd $username -g players

sudo passwd $username

if [[ $players == "" ]]; then
    printf "\nYou are the host...\n"
    sudo usermod -a -G players-host $username
fi

sudo -u $username /bin/bash
