#!/bin/bash

function help()
{
    echo "This is help cmd"
}

function close()
{
    echo "This is close cmd"
}
function print()
{
    echo "First param is : $1" 
    echo "Second param is : $2" 
}

case $1 in
    "help")
        help
        ;;

    "close")
        close 
        ;;
    "print")
        print first second 
        ;;
     *)
       echo "Sorry, cmd error"
        ;;
esac
