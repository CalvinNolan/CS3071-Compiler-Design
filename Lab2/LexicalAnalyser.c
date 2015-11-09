//
//  LexicalAnalyser.c
//  CS3071Lab2
//
//  Created by Calvin Nolan on 17/10/2015.
//  Copyright © 2015 Calvin Nolan. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// A function that takes in a character (0-9 | a - f | A - F) and returns its base 10 value.
int atoi_base_16(char c) {
    if(c >= '0' && c <= '9') {
        return c - 48;
    } else if(c == 'a' || c == 'A') {
        return 10;
    } else if(c == 'b' || c == 'B') {
        return 11;
    } else if(c == 'c' || c == 'C') {
        return 12;
    } else if(c == 'd' || c == 'D') {
        return 13;
    } else if(c == 'e' || c == 'E') {
        return 14;
    } else if(c == 'f' || c == 'F') {
        return 15;
    } else {
        return 0;
    }
}

// A function that takes in a string representing a 32bit hexadecimal, octal or signed integer value and converts it to its 32bit integer value.
void lexicalAnalyser(char * stringValue) {
    
    int i;
    int leadingZeroesCount = 0;
    char firstValue = ' ';
    int sign = 0;
    int base = 8;
    int totalCount = 0;
    
    int leadingZeroes = 0;
    int error = 0;
    
    // First Pass.
    // This pass determines the length of the input value, if it's valid and it's base.
    for(i = 0; i < strlen(stringValue) && error == 0; i++) {
        char characterI = stringValue[i];
        
        // Check if the first character is a sign.
        // State 2.
        if((i == 0) && (characterI == '-')) {
            sign = -1;
            continue;
        } else if ((i == 0) && (characterI == '+')) {
            sign = 1;
            continue;
        }
        
        // Check if the first value is a leading zero.
        // State 3.
        if((totalCount == 0) && characterI == '0') {
            leadingZeroes = 1;
            leadingZeroesCount++;
            totalCount++;
        }
        // Check if this character is a leading zero.
        // State 3.
        else if(leadingZeroes == 1 && characterI == '0') {
            leadingZeroesCount++;
            totalCount++;
        }
        // Mark the end of all leading zeroes.
        // Transition out of state 3.
        else if(firstValue == ' '){
            leadingZeroes = 0;
            firstValue = characterI;
        }
        
        
        
        // Check the base value
        if(i == (strlen(stringValue)-1)) {
            // State 8.
            if(characterI == 'h' || characterI == 'H') {
                base = 16;
            } 
            // State 7.
            else if((characterI == 'b' || characterI == 'B') && base != 16 && base != 10) {
                base = 8;
            } 
            // State 5.
            else if((characterI >= 48 && characterI <= 57) && base != 16) {
                base = 10;
                totalCount++;
            } else {
                error = 1;
                printf("Error, incompatible base! \n");
            }
            
            if(sign != 0 && base != 10) {
                error = 1;
                printf("Error, You can only specify a sign for base 10 values!\n");
            }
            
            continue;
        }
        
        // Check that the value is valid and which base we are currently satisfying.
        if(leadingZeroes == 0) {
            totalCount++;
            // State 4.
            if((characterI >= 48 && characterI <= 55) && base != 10 && base != 16){
                base = 8;
            } 
            // State 5.
            else if((characterI >= 48 && characterI <= 57) && base != 16) {
                base = 10;
            } 
            // State 6.
            else if((characterI >= 48 && characterI <= 57)
                      || (characterI >= 65 && characterI <= 70)
                      || (characterI >= 97 && characterI <= 102)){
                base = 16;
            } else {
                error = 1;
                printf("Error, invalid input '%c'! \n", characterI);
            }
        }
    }
    
    if(error != 0){
        return;
    }
    
    if(leadingZeroes == 1){
        printf("Lexical token (%s Constant, %d) \n", "Signed Integer", 0);
        return;
    }
    
    int count = totalCount - leadingZeroesCount;
    int negative = 0;
    int overflowCheck = 0;
    
    //Second Pass.
    //Part I.
    //Checks if the value will be negative and if we will have overflow for base 8 or 12.
    if(base == 8) {
        if(count == 11 && firstValue > '3') {
            printf("Error, Overflow in base 8! \n");
            return;
        } else if(count > 11) {
            printf("Error, Overflow in base 8! \n");
            return;
        }
        
        if(count == 11 && firstValue > '1') {
            negative = 1;
        }
    } else if(base == 16) {
        if(count > 8) {
            printf("Error, Overflow in base 16! \n");
            return;
        }
        
        if(count == 8 && firstValue > '7') {
            negative = 1;
        }
        
    } else if(base == 10) {
        if(count == 10 && firstValue > '2') {
            printf("Error, Overflow in base 10! \n");
            return;
        } else if(count > 10) {
            printf("Error, Overflow in base 10! \n");
            return;
        }
        
        if(count == 10 && firstValue == '2') {
            overflowCheck = 1;
        }
    }
    
    //Part II.
    //Calculates the integer value.
    int32_t total = 0;
    if(negative == 1) {
        total = -1;
    }
    
    if(sign != 0) {
        leadingZeroesCount++;
        totalCount++;
    }
    
    for(i = leadingZeroesCount; i < totalCount; i++) {
        char characterI = stringValue[i];
        if(base == 8) {
            int32_t currentChar = characterI - 48;
            if(negative == 1) {
                total = (total * 8) + (currentChar - 7);
            } else {
                total = (total * 8) + currentChar;
            }
        } else if(base == 16) {
            int32_t currentChar = atoi_base_16(characterI);
            if(negative == 1) {
                total = (total * 16) + (currentChar - 15);
            } else {
                total = (total * 16) + currentChar;
            }
        } else if(base == 10) {
            int32_t currentChar = characterI - 48;
            if(overflowCheck == 1){
                switch (i - leadingZeroesCount + 1) {
                    case 1:
                        if(currentChar > 2) {
                            printf("Error, Overflow in base 10! \n");
                            return;
                        } else if(currentChar < 2) {
                            overflowCheck = 0;
                        }
                        break;
                    case 2:
                        if(currentChar > 1) {
                            printf("Error, Overflow in base 10! \n");
                            return;
                        } else if(currentChar < 1) {
                            overflowCheck = 0;
                        }
                        break;
                    case 3:
                        if(currentChar > 4) {
                            printf("Error, Overflow in base 10! \n");
                            return;
                        } else if(currentChar < 4) {
                            overflowCheck = 0;
                        }
                        break;
                    case 4:
                        if(currentChar > 7) {
                            printf("Error, Overflow in base 10! \n");
                            return;
                        } else if(currentChar < 7) {
                            overflowCheck = 0;
                        }
                        break;
                    case 5:
                        if(currentChar > 4) {
                            printf("Error, Overflow in base 10! \n");
                            return;
                        } else if(currentChar < 4) {
                            overflowCheck = 0;
                        }
                        break;
                    case 6:
                        if(currentChar > 8) {
                            printf("Error, Overflow in base 10! \n");
                            return;
                        } else if(currentChar < 8) {
                            overflowCheck = 0;
                        }
                        break;
                    case 7:
                        if(currentChar > 3) {
                            printf("Error, Overflow in base 10! \n");
                            return;
                        } else if(currentChar < 3) {
                            overflowCheck = 0;
                        }
                        break;
                    case 8:
                        if(currentChar > 6) {
                            printf("Error, Overflow in base 10! \n");
                            return;
                        } else if(currentChar < 6) {
                            overflowCheck = 0;
                        }
                        break;
                    case 9:
                        if(currentChar > 4) {
                            printf("Error, Overflow in base 10! \n");
                            return;
                        } else if(currentChar < 4) {
                            overflowCheck = 0;
                        }
                        break;
                    case 10:
                        if(currentChar > 7) {
                            printf("Error, Overflow in base 10! \n");
                            return;
                        } else if(currentChar < 7) {
                            overflowCheck = 0;
                        }
                        break;
                }
            }
            
            total = (total * 10) + currentChar;
        }
    }
    if(sign == -1) {
        total = total * -1;
    }
    if(negative == 1) {
        total--;
    }
    
    char * type = "";
    if(base == 8) {
        type = "Octal";
    } else if(base == 10) {
        type = "Signed Integer";
    } else {
        type = "Hexadecimal";
    }
    
    printf("Lexical token (%s Constant, %d) \n", type, total);
    return;
}

int main(int argc, const char * argv[]) {
    
    if(argc < 2) {
        printf("Error, You have to input your values when executing the program! \n");
        return 0;
    }
    int j;
    
    for(j = 1; j < argc; j++) {
        lexicalAnalyser((char *) argv[j]);
    }
    
    return 0;
}
