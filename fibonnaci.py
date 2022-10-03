
## Program to print the fibonacci sequence to the Nth degree

## Algorithm:
## 1. Add the last two numbers in the sequence
## 2. Create the next number from that sum
## 3. Repeat until the user's number has been reached

## Author: David Kelly
## Date: 23 Feb 2021


first = 1
second = 0
i = 0
n = 1
result = 0

## Loop until user enters a valid amount of loops.
while n > 0:

    ## Prompt user to enter the amount of times this code will loop.
    n = int(input("How many cycles? "))

    ## Error check: user must enter number greater than zero.
    if n < 1:
        print("Sorry, must be more than 0\nTry again")
    else:
        break

## Loop this code 'n' amount of times. (User entered value for 'n' earlier)
while i < n:

    ## Store the answer to this sum in 'result'.
    result = first + second

    ## Display the result of the sum.
    print(result)

    ## The value from 'first' is now stored in 'second'.
    second = first

    ## The value from 'result' is now stored in 'first'.
    first = result

    ## Increment 'i' to keep the loop running.
    i = i + 1
