#!/bin/bash

echo "Login As..."

read username

su $username -s /bin/bash
