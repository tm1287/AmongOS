#!/bin/bash

players="$(members players)"

usernamegood=false

while [ "$usernamegood" = false ]
do
    echo "Login As..."

    read username

    if grep -qow "$username\b" <<<"$players"; then usernamegood=true; else echo "Error: User does not exist"; fi
done



su $username -s /bin/bash
