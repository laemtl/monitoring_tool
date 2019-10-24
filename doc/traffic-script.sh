#! /bin/bash

# Randomly permute the arguments and put them in array 'outarray'
function perm
{
    outarray=( "$@" )

    # The algorithm used is the Fisher-Yates Shuffle
    # (https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle),
    # also known as the Knuth Shuffle.

    # Loop down through 'outarray', swapping the item at the current index
    # with a random item chosen from the array up to (and including) that
    # index
    local idx rand_idx tmp
    for ((idx=$#-1; idx>0 ; idx--)) ; do
        rand_idx=$(( RANDOM % (idx+1) ))
        # Swap if the randomly chosen item is not the current item
        if (( rand_idx != idx )) ; then
            tmp=${outarray[idx]}
            outarray[idx]=${outarray[rand_idx]}
            outarray[rand_idx]=$tmp
        fi
    done
}

inarray=( 'server A' 'server B' 'server C' )
inarray=( 'server A' 'server B' 'server C' )
inarray=( 'server A' 'server B' 'server C' )

sudo ScapyTrafficGenerator -X http -r '-X 1 -F /home/lae/Desktop/test.txt -i wlp1s0 -s 1.1.1.1 -S 33333 -d 2.2.2.2 -D 80'  

# Declare 'outarray' for use by 'perm'
declare -a outarray

perm "${inarray[@]}"

# Display the contents of 'outarray'
declare -p outarray