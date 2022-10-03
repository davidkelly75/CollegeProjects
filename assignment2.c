/*
Assignment 2

This is a security authentication program based around a 4 digit access code.

The user is given 5 options:

    Option 1 = Enter a 4 digit code.
    Option 2 = Encrypt the entered code and then compare it to the access code.
    Option 3 = Decrypt the now encrypted code.
    Option 4 = Display no. of (i) correct and (ii) incorrect attempts at matching the access code.
    Option 5 = Exit program.

NOTE: 
For option 2,
- if a code has not been entered yet, nothing will be encrypted.
- the code will only be encrypted once;
  this program does not allow the user to encrypt an already encrypted code.

NOTE:
For option 3,
- if a code has not been entered yet, nothing will be decrypted.
- the code will only be decrypted once;
  this program does not allow the user to decrypt an already decrypted code.

Each menu option is called in main() and implemented in a separate function if selected by the user.

Each function passes ALL parameters by reference.
All reading and writing to and from arrays is done using pointer notation.

All input by the user is checked for errors.

Author: David Kelly
Date: 7 Mar 2021
*/

#include<stdio.h>

#define SIZE 4

void first_option(char [], char *, char *);
void second_option(char [], char [], char *, char *, int *, int *);
void third_option(char [], char *, char *);
void fourth_option(int *, int *);
void fifth_option(char *);
void error_check(char *, char *);

int main()
{
    char access_code[SIZE] = {'4', '5', '2', '3'};
    char user_code[SIZE];
    char user_code_entered = 'n';
    char option;
    char confirm_exit = 'n';
    char encrypted = 'n';
    int i;
    int correct_code = 0;
    int incorrect_code = 0;

    // Display options menu
    printf("\n1. Enter any code");
    printf("\n2. Encrypt code entered and verify if correct");
    printf("\n3. Decrypt code");
    printf("\n4. Display no. of times the encrypted code entered matches "
    "authorised code (i) Correctly and (ii) Incorrectly");
    printf("\n5. Exit program\n");

    // Loop until user enters 'y' to confirm their decision to exit.
    while (confirm_exit != 'y')
    {
        // Prompt user to choose an option
        printf("\nPlease choose one of the options (1-5) listed above\n");

        // Read in option choice from the user (1-5)
        scanf("%c", &option);

        // Clear the input buffer
        while (getchar() != '\n');

        // Switch statement for each option case (1-5)
        switch(option)
        {
            case '1':
            {
                // Run first_option() to allow user enter any code.
                // Pass three parameters:
                // 1. the code the user has entered.
                // 2. the variable which indicates whether or not the user has entered a code yet.
                // 3. the variable to indicate whether the code is encrypted or not.
                first_option(user_code, &user_code_entered, &encrypted);

                break;
            }// End case 1

            case '2':
            {
                // Run second_option() to encrypt the user's entered code.
                // Pass six parameters:
                // 1. the code the user has entered.
                // 2. the default access code.
                // 3. the variable which indicates whether or not the user has entered a code yet.
                // 4. variable which indicates whether or not the code is encrypted already.
                // 5. variable to count amount of times user's code is correct.
                // 6. variable to count amount of times user's code is incorrect.
                second_option(user_code, access_code, &user_code_entered, &encrypted, &correct_code, &incorrect_code);

                break;
            }// End case 2

            case '3':
            {
                // Run third_option() to decrypt the user's entered code.
                // Pass three parameters:
                // 1. the code the user has entered.
                // 2. the variable which indicates whether or not the user has entered a code yet.
                // 3. the variable to indicate whether the code is encrypted or not.
                third_option(user_code, &user_code_entered, &encrypted);

                break;
            }// End case 3

            case '4':
            {
                // Run fourth_option() to display amount of correct and incorrect entries.
                // Pass two parameters:
                // 1. variable to store the amount of correct entries.
                // 2. variable to store the amount of incorrect entries.
                fourth_option(&correct_code, &incorrect_code);

                break;
            }// End case 4

            case '5':
            {
                // Run fifth_option() to allow user to confirm decision to exit.
                // Pass one parameter: variable to store user's decision.
                fifth_option(&confirm_exit);

                break;
            }// End case 5

            default:
            {
                printf("\nERROR: Invalid option entered! Choose only from 1 to 5\n");

                break;
            }// End default
        
        }// End switch
    
    }// End while

    printf("\nGoodbye\n\n");

    return 0;
}// End main()


// Function to carry out option 1.
// Allow user to enter a code.
void first_option(char code_array[], char *code_entered_yet, char *encrypted_yet)
{
    int i;
    char invalid_digit = 'y'; // Initiate invalid_digit to 'y' (yes) to enter the while loop.

    // Loop until the user enters a valid code without error.
    while (invalid_digit == 'y')
    {
        // Prompt user to enter 4 digit code.
        printf("\nEnter a %d digit code (Press enter key after each digit)\n", SIZE);

        // Loop to enter data into the array.
        for (i = 0; i < SIZE; i++)
        {
            // Read user's numbers into the array.
            scanf("%c", code_array + i);

            // Clear the input buffer.
            while (getchar() != '\n');

            // Check the digit that has just been entered to ensure it's a valid digit.
            error_check(code_array+i, &invalid_digit);

        }// End for

        // If: any digits weren't valid, display error message
        if (invalid_digit == 'y')
        {
            // Display message to tell user they entered an invalid digit.
            printf("\nERROR: You have entered an invalid digit.\nTry again.\n");

        }// End if

        // Else: all digits were valid, so update the necessary variables.
        else
        {
            // Update 'user_code_entered' by reference with 'y' (yes) to confirm a code has been entered.
            // Used for option 2 and 3.
            *code_entered_yet = 'y';

            // Update 'encrypted' by reference with 'n' (no) because the new code is decrypted by default.
            *encrypted_yet = 'n';

        }// End else

    }// End while

}// End first_option()


// Function to carry out option 2.
// Encrypt user's code.
void second_option(char user_array[], char original_code[], char *code_entered_yet, char *encrypted_yet, int *correct, int *incorrect)
{
    char temp;
    int i;
    char match = 'y';

    // If: the user has not entered a code yet, tell them to enter a code first.
    if (*code_entered_yet == 'n')
    {
        // Display message redirecting user to option 1.
        printf("\nYou haven't entered a %d digit code yet.\
        \nChoose option 1 to enter a code before you select option 2.\n", SIZE);

    }// End if

    // Else if: the code is already encrypted, then leave the code as it is.
    else if (*encrypted_yet == 'y')
    {
        // Display message to tell the user their code is already encrypted.
        printf("\nCannot encrypt code, your code is already encrypted."
        "\nChoose option 3 to decrypt.\n");

    }// End else if

    // Else: a code has been entered, so encrypt the code.
    else
    {
        // Swap the 1st number with the 3rd number.
        temp = *(user_array + 2);
        *(user_array + 2) = *user_array;
        *user_array = temp;

        // Swap the 2nd number with the 4th number.
        temp = *(user_array + 3);
        *(user_array + 3) = *(user_array + 1);
        *(user_array + 1) = temp;

        // Add 1 to each number, unless the number is 9.
        for (i = 0; i < SIZE; i++)
        {
            // If: any number is 9, change it to 0.
            if (*(user_array + i) == '9')
            {
                *(user_array + i) = '0';

            }// End if

            // Else: the number is not 9, so add 1.
            else
            {
                *(user_array + i) = *(user_array + i) + 1;

            }// End else

        }// End for

        // Update 'encrypted' by reference to 'y' (yes).
        *encrypted_yet = 'y';

        // Let the user know that their code is now encrypted.
        printf("\nYour %d digit code has now been encrypted.\n", SIZE);

        // Check does the encrypted code match the access code.
        for (i = 0; i < SIZE; i++)
        {
            // If: any digit does not match, end the loop.
            if (*(user_array + i) != *(original_code + i))
            {
                // 'match' is a variable to record if any digit does not match.
                // Set match to 'n' (no)
                match = 'n';

                // Reset 'i' to break the loop
                i = SIZE;

            }// End if

        }// End for

        // CODE TESTING - For loop to display encrypted array.
        /*
        for (i = 0; i < SIZE; i++)
        {
            printf("%c ", *(user_array + i));
        
        }// End for
        */

        // Compare user's code with the access code.

        // If: both codes match, display "Correct Code entered" message.
        if (match == 'y')
        {
            printf("\nCorrect Code entered\n");

            // Update by reference the amount of correct code entries (used in option 4).
            *correct = *correct + 1;

        }// End if

        // Else: both codes did not match, display "Wrong Code entered" message.
        else
        {
            printf("\nWrong Code entered\n");
            
            // Update by reference the amount of incorrect code entries (used in option 4).
            *incorrect = *incorrect + 1;

        }// End else

    }// End else
    
}// End second_option()


// Function to carry out option 3.
// Decrypt the entered code.
void third_option(char user_array[], char *code_entered_yet, char *encrypted_yet)
{
    char temp;
    int i;

    // If: the user has not entered a code yet, tell them to enter a code first.
    if (*code_entered_yet == 'n')
    {
        // Display message redirecting user to option 1.
        printf("\nYou haven't entered a %d digit code yet.\
        \nChoose option 1 to enter a code before you select option 3.\n", SIZE);

    }// End if

    // Else if: the code is not encrypted yet, then leave the code as it is.
    else if (*encrypted_yet == 'n')
    {
        //
        printf("\nCannot decrypt code, your code is already decrypted."
        "\nChoose option 2 to encrypt.\n");

    }// End else if

    // Else: a code has been entered, so decrypt the code.
    else
    {
        // Subtract 1 from each number, unless the number is 0.
        for (i = 0; i < SIZE; i++)
        {
            // If: any number is 0, change it to 9.
            if (*(user_array + i) == '0')
            {
                *(user_array + i) = '9';

            }// End if

            // Else: the number is not 0, so subtract 1.
            else
            {
                *(user_array + i) = *(user_array + i) - 1;

            }// End else

        }// End for

        // Swap the 1st number with the 3rd number.
        temp = *(user_array + 2);
        *(user_array + 2) = *user_array;
        *user_array = temp;

        // Swap the 2nd number with the 4th number.
        temp = *(user_array + 3);
        *(user_array + 3) = *(user_array + 1);
        *(user_array + 1) = temp;

        // Update 'encrypted' by reference to 'n' (no).
        *encrypted_yet = 'n';

        // Let the user know that their code is now decrypted.
        printf("\nYour %d digit code has now been decrypted.\n", SIZE);

        // CODE TESTING - For loop to display decrypted array.
        /*
        for (i = 0; i < SIZE; i++)
        {
            printf("%c ", *(user_array + i));
        
        }// End for
        */

    }// End else

}// End third_option()


// Function to carry out option 4.
// Display no. of times the user's code matched access code (i) correctly and (ii) incorrectly
void fourth_option(int *correct, int *incorrect)
{
    printf("\n(i) %d correct entries today", *correct);
    printf("\n(ii) %d incorrect entries today\n", *incorrect);

}// End fourth_option()


// Function to carry out option 5.
// Confirm the user's decision to exit program.
void fifth_option(char *yes_no)
{
    // Prompt user to confirm their exit.
    printf("\nYou have chosen to quit. Are you sure? (y/n)\n");

    // Read in user's decision (y/n) and store in 'yes_no' variable.
    scanf("%c", yes_no);

    // Clear the input buffer.
    while (getchar() != '\n');

    // Switch statement to error check the y/n input of the user.
    switch (*yes_no)
    {
        // If 'n' was entered, do nothing because it's a valid input.
        case ('n'):
        {
            break;
        }// End case 'n'

        // If 'y' was entered, do nothing because it's a valid input.
        case ('y'):
        {
            break;
        }// End case 'y'

        default: // The user entered something other than 'y' or 'n'.
        {
            // Display message to tell the user to type 'y' or 'n' next time.
            printf("\nERROR: Invalid input."
            "\nNext time, type 'y' for yes, or type 'n' for no."
            "\nChoose option 5 again if you still wish to exit.\n");

        }// End default

    }// End switch

}// End fifth_option()


// Function to check if a user input is a valid integer or not.
void error_check(char *user_num, char *invalid)
{
    // Switch statement to display error message if any digit entered is not 0-9.
    switch(*user_num)
    {
        case '0':
        {
            // Update 'invalid_digit' by reference to 'n' (no).
            *invalid = 'n';

            break;
        }// End case '0'

        case '1':
        {
            // Update 'invalid_digit' by reference to 'n' (no).
            *invalid = 'n';

            break;
        }// End case '1'

        case '2':
        {
            // Update 'invalid_digit' by reference to 'n' (no).
            *invalid = 'n';

            break;
        }// End case '2'

        case '3':
        {
            // Update 'invalid_digit' by reference to 'n' (no).
            *invalid = 'n';

            break;
        }// End case '3'

        case '4':
        {
            // Update 'invalid_digit' by reference to 'n' (no).
            *invalid = 'n';

            break;
        }// End case '4'

        case '5':
        {
            // Update 'invalid_digit' by reference to 'n' (no).
            *invalid = 'n';

            break;
        }// End case '5'

        case '6':
        {
            // Update 'invalid_digit' by reference to 'n' (no).
            *invalid = 'n';

            break;
        }// End case '6'

        case '7':
        {
            // Update 'invalid_digit' by reference to 'n' (no).
            *invalid = 'n';

            break;
        }// End case '7'

        case '8':
        {
            // Update 'invalid_digit' by reference to 'n' (no).
            *invalid = 'n';

            break;
        }// End case '8'

        case '9':
        {
            // Update 'invalid_digit' by reference to 'y' (no).
            *invalid = 'n';

            break;
        }// End case '9'

        default: // The user entered an invalid digit.
        {
            // Update 'invalid_digit' by reference to 'y' (yes).
            *invalid = 'y';

            break;
        }// End default

    }// End switch

}// End error_check()