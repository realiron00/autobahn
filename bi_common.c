#include "autobahn_common.h"

/**
 * @brief Initialize a new big integer.
 * 
 * This function initializes a new big integer structure, allocating memory for
 * the structure and its associated digit array. The sign of the big integer is
 * set to POSITIVE by default, and the maximum number of digits 'dmax' for the
 * big integer is assigned. The digit array is also initialized with zeros.
 *
 * @param x Pointer to a pointer to a bi structure (output parameter) where the
 *          newly created big integer will be stored.
 * @param dmax Maximum number of digits for the big integer to accommodate.
 *             This value determines the size of the digit array.
 */
void bi_new(bi** x, bi_uword dmax)
{
	//Delete the value in the structure if there is any
	if (*x != NULL)
		bi_delete(x);

	//Allocate memory for the structure x
	*x = (bi*)malloc(sizeof(bi));
	(*x)->sign = POSITIVE;
	(*x)->dmax = dmax;

	//Allocate memory for the array a
	(*x)->a = (bi_uword*)calloc(dmax, sizeof(bi_uword));
}

/**
 * @brief Free the memory used by a big integer.
 * 
 * This function deallocates the memory used by a big integer structure and its
 * associated array of digits, pointed to by 'x'. It also sets 'x' to NULL to
 * indicate that the big integer is no longer valid.
 *
 * @param x Pointer to a pointer to a bi structure (input/output parameter).
 *          Upon successful deallocation, 'x' is set to NULL.
 */
void bi_delete(bi** x)
{
	//just return if the structure is already empty
	if (*x == NULL)
		return;

	//free the memory of the structure
	free((*x)->a);
	free(*x);

	//Insert NULL value to prevent dangling pointer
	*x = NULL;
}

/**
 * @brief Refine the big integer by removing leading zero digits.
 * 
 * Refining a big integer means removing any leading zero digits from its
 * representation to make it more compact and efficient. This function
 * iterates through the digits of the big integer 'x', starting from the
 * most significant digit, and removes any leading zero digits until a
 * non-zero digit or the last digit is encountered. The 'dmax' field of 'x'
 * is updated to reflect the new maximum number of digits after refining.
 * 
 * @param x Pointer to a bi structure representing the big integer to refine.
 */
void bi_refine(bi* x)
{
	//just return if the structure is empty
	if (x == NULL)
		return;

	//Find the index of the first non-zero value from the end of the array
	bi_uword new_dmax = x->dmax;

	//Check from the end of the array to the beginning until a non-zero value is found
	while (new_dmax > 1) {
		//Save the index of the first non-zero value to new_dmax
		if (x->a[new_dmax - 1] != 0)
			break;
		new_dmax--;
	}

	//If the index of the first non-zero value is different from the original dmax, change the dmax and reallocate the memory
	if (x->dmax != new_dmax) {
		x->dmax = new_dmax;
		x->a = (bi_uword*)realloc(x->a, sizeof(bi_uword) * new_dmax);
	}

	//If the value of the first non-zero value is 0, change the sign to POSITIVE(a=0)
	if ((x->dmax == 1) && (x->a[0] == 0)) {
		x->sign = POSITIVE;
	}
}

/**
 * @brief Display the big integer in binary format.
 * 
 * This function displays the binary representation of the big integer 'x' to
 * the standard output (console). Each digit of the big integer is represented
 * as a sequence of binary digits (bits), starting from the most significant
 * digit and ending with the least significant digit. Leading zero bits are
 * included to maintain the specified maximum number of digits ('dmax'). The
 * binary digits are separated by spaces for readability.
 * 
 * For example, if 'x' is 9 (in decimal) with 'dmax' set to 8, the output
 * will be "00001001" in binary format.
 * 
 * @param x Pointer to a bi structure representing the big integer to display.
 */
void bi_show_bin(const bi* x)
{
    // Check if the "bi" is negative and print a minus sign if it is.
    if (x->sign)
        printf("-");

    // Display the number in binary notation.
    printf("0b");

    // Iterate through the elements of the "a" array in the "bi" structure.
    for (int i = x->dmax - 1; i >= 0; i--)
    {
        // Iterate through the bits within each element.
        for (int j = sizeof(bi_word) * 8 - 1; j >= 0; j--)
        {
            // Print each bit of the "bi" number by shifting and masking.
            printf("%d", (x->a[i] >> j) & 1);
        }
    }

    // Print a newline character to end the output.
    printf("\n");
}

//TODO DH 구현 예정
/**
 * @brief Display the big integer in hexadecimal format.
 * 
 * This function displays the hexadecimal representation of the big integer 'x'
 * to the standard output (console). Each digit of the big integer is represented
 * as a hexadecimal character (0-9, A-F), starting from the most significant digit
 * and ending with the least significant digit. Leading zero digits are included
 * to maintain the specified maximum number of digits ('dmax').
 * 
 * For example, if 'x' is 255 (in decimal) with 'dmax' set to 4, the output
 * will be "00FF" in hexadecimal format.
 * 
 * @param x Pointer to a bi structure representing the big integer to display.
 */
void bi_show_hex(const bi* x) 
{
	// Check negative
    if (x->sign) 
		printf("-");

	// Display in hexadecimal notation.
    printf("0x");
    
	// Print to hexadecimal.
    for (int i = x->dmax - 1; i >= 0; i--)
        printf("%x", x->a[i]);

    printf("\n");
}

/**
 * @brief Set a big integer from an array of words.
 * 
 * This function initializes a big integer 'x' with the values from an array of
 * 'dmax' words ('arr') representing the big integer's digits. The 'sign'
 * parameter determines whether 'x' is positive or negative. The function allocates
 * memory for the big integer 'x' and copies the values from the 'arr' into its
 * digit array. If 'dmax' is greater than the actual number of digits in 'arr',
 * leading digits are set to zero.
 * 
 * @param x Pointer to a pointer to a bi structure (output parameter) where the
 *          newly created big integer will be stored.
 * @param arr Array of bi_uword representing the big integer's digits.
 * @param sign Sign of the big integer (POSITIVE or NEGATIVE).
 * @param dmax Maximum number of digits for the big integer. This value determines
 *             the size of the big integer's digit array.
 */
void bi_set_by_arr(bi** x, const bi_uword* a, sign sign, bi_uword dmax)
{
    // Create a new big integer with the specified maximum number of words.
    bi_new(x, dmax); 
    
    // Set the sign of the big integer.
    (*x)->sign = sign; 
    
    // Copy the elements from the input array to the big integer's data array.
    for (bi_word i = 0; i < dmax; i++)
        (*x)->a[i] = a[i];
}

/**
 * @brief Set a big integer from a hexadecimal string representation.
 * 
 * This function initializes a big integer 'x' with the values parsed from a hexadecimal
 * format string 'str'. The 'sign' parameter determines whether 'x' is positive or negative.
 * The function allocates memory for the big integer 'x' and parses the string 'str' to populate
 * its digit array. Each chunk of hexadecimal digits in 'str' (with a size of 2 characters per
 * chunk) is converted to a bi_uword and stored in the big integer's array. If 'str' contains
 * characters that are not valid hexadecimal digits, the behavior is undefined.
 * 
 * @param x Pointer to a pointer to a bi structure (output parameter) where the newly created
 *          big integer will be stored.
 * @param str Hexadecimal string representation of the big integer.
 * @param sign Sign of the big integer (POSITIVE or NEGATIVE).
 */
static void bi_set_by_str_hex(bi** x, const char* str, sign sign)
{
    // Calculate the length of the input string 'str'.
    bi_uword len = (bi_uword)strlen(str);

    // Calculate the number of bi_uword elements required to store the hexadecimal digits.
    bi_uword new_dmax = (len - 1) / (sizeof(bi_uword) * 2) + 1;

    // Allocate memory for the big integer.
    bi_new(x, new_dmax);

    // Check if memory allocation was successful.
    if (*x == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(1);
    }

    // Set the sign of the big integer.
    (*x)->sign = sign;

    for (int i = 0; i < (*x)->dmax; i++) 
    {
        // Calculate the size of each chunk of hexadecimal digits (2 characters per chunk).
        bi_uword chunk_size = sizeof(bi_uword) * 2;

        // Create a buffer to store the chunk of hexadecimal digits.
        char* buf = malloc(chunk_size + 1);
        
        // Copy a chunk of hexadecimal digits from the input string to buf.
        strncpy(buf, str + i * chunk_size, chunk_size);

        // Null-terminate the buffer to ensure proper string conversion.
        buf[chunk_size] = '\0';

        // Convert the chunk to a bi_uword and store it in the big integer's array.
        (*x)->a[i] = strtoul(buf, NULL, HEXADECIMAL);
    }
}

//TODO 나중에 구현하자
static void bi_set_by_str_dec(bi** x, const char* str, sign sign)
{
	fprintf(stderr, "지원하지 않는 기능입니다.\n");
    exit(1);
}

//TODO 나중에 구현하자
static void bi_set_by_str_bin(bi** x, const char* str, sign sign)
{
	fprintf(stderr, "지원하지 않는 기능입니다.\n");
    exit(1);
}

/**
 * @brief Set a big integer from a string representation.
 * 
 * This function initializes a big integer 'x' with the values parsed from a string
 * 'str' representing the big integer in the specified numerical 'base'. The 'sign'
 * parameter determines whether 'x' is positive or negative. The function allocates
 * memory for the big integer 'x' and parses the string 'str' to populate its digit
 * array. The 'base' parameter indicates the numerical base of 'str' (BINARY, DECIMAL,
 * or HEXADECIMAL). If 'str' contains characters that are not valid digits in the
 * specified base, the behavior is undefined.
 * 
 * @param x Pointer to a pointer to a bi structure (output parameter) where the
 *          newly created big integer will be stored.
 * @param str String representation of the big integer.
 * @param sign Sign of the big integer (POSITIVE or NEGATIVE).
 * @param base Numerical base of the string representation (BINARY, DECIMAL, or HEXADECIMAL).
 */
void bi_set_by_str(bi** x, const char* str, sign sign, base base) 
{	
	// Call the function for hexadecimal input.
    if (base == HEXADECIMAL)
        bi_set_by_str_hex(x, str, sign);

	// Call the function for decimal input.
    else if (base == DECIMAL)
        bi_set_by_str_dec(x, str, sign);

	// Call the function for binary input.
    else if (base == BINARY)
        bi_set_by_str_bin(x, str, sign);

	// Print an error message and exit the program with an error code.
	else
	{
        fprintf(stderr, "Error: Invalid base specified for string representation.\n");
        exit(1);
	}
}

/**
 * @brief Set a big integer to zero.
 * 
 * This function initializes a big integer 'x' and sets its value to zero.
 * It allocates memory for the big integer structure, sets the sign to
 * POSITIVE, and initializes the digit array to all zeros. After calling
 * this function, 'x' will represent the value zero with a specified maximum
 * number of digits ('dmax').
 * 
 * @param x Pointer to a pointer to a bi structure (output parameter) where
 *          the newly created big integer representing zero will be stored.
 */
void bi_set_zero(bi** x)
{
    // Allocate memory for a new "bi" (big integer) and initialize it to 1.
    bi_new(x, 1);

    // Set the sign of the "bi" to positive (0 means positive, 1 means negative).
    (*x)->sign = 0;

    // Set the value of the "bi" to 0.
    (*x)->a[0] = 0;
}

/**
 * @brief Set a big integer to the value one.
 * 
 * This function initializes a big integer 'x' and sets its value to one.
 * It allocates memory for the big integer structure, sets the sign to
 * POSITIVE, and initializes the digit array with a single digit '1'.
 * After calling this function, 'x' will represent the value one with a
 * specified maximum number of digits ('dmax').
 * 
 * @param x Pointer to a pointer to a bi structure (output parameter) where
 *          the newly created big integer representing one will be stored.
 */
void bi_set_one(bi** x)
{
    // Allocate memory for a new "bi" (big integer) and initialize it to 1.
    bi_new(x, 1);

    // Set the sign of the "bi" to positive (0 means positive, 1 means negative).
    (*x)->sign = 0;

    // Set the value of the "bi" to 1.
    (*x)->a[0] = 1;
}

/**
 * @brief Copy the value of one big integer to another.
 * 
 * This function creates a copy of the big integer 'x' and stores it in the
 * big integer 'y'. It allocates memory for the new big integer 'y' and
 * duplicates the value and properties (sign, dmax, digit array) of 'x'.
 * After calling this function, 'y' will be an independent copy of 'x'.
 * 
 * @param y Pointer to a pointer to a bi structure (output parameter) where
 *          the copy of the big integer 'x' will be stored.
 * @param x Pointer to a bi structure representing the big integer to be copied.
 */
void bi_cpy(bi** y, const bi* x)
{
	//Delete the value in the structure if there is any in y
	if (*y != NULL)
		bi_delete(y);

	//Create a new structure y with the same dmax as x
	bi_new(y, x->dmax);

	//Copy the data from structure x to structure y
	(*y)->sign = x->sign;
	for (int i = 0; i < x->dmax; i++)
		(*y)->a[i] = x->a[i];
}

/**
 * @brief Compare two big integers (absolute values) and return the result.
 *
 * This function compares the absolute values of two big integers 'x' and 'y'
 * and returns the result as a bi_word. The bi_word represents the relationship
 * between the magnitudes of the two big integers.
 *
 * @param x Pointer to the first big integer to compare.
 * @param y Pointer to the second big integer to compare.
 * @return bi_word Result of the comparison:
 *                - Positive value (1) if |x| is greater than |y|.
 *                - Negative value (-1) if |y| is greater than |x|.
 *                - Zero (0) if |x| and |y| are equal in magnitude.
 */
static bi_word bi_cmpABS(const bi* x, const bi* y)
{
	bi_word x_dmax = x->dmax;
	bi_word y_dmax = y->dmax;

	//Return 1 if x's dmax is bigger than y's dmax
	if (x_dmax > y_dmax)
		return 1;
	
	//Return -1 if y's dmax is bigger than x's dmax
	else if (x_dmax < y_dmax)
		return -1;

	//if x_dmax == y_dmax, compare each digit
	for (int i = x_dmax-1; i >= 0; i--) {
		//Return 1 if the value of the digit in x is bigger than the value of the digit in y
		if (x->a[i] > y->a[i])
			return 1;

		//Return -1 if the value of the digit in y is bigger than the value of the digit in x
		else if (x->a[i] < y->a[i])
			return -1;
	}
	//if all the digits are the same, return 0
	return 0;
}

/**
 * @brief Compare two big integers and return the result
 * 
 * @param x Pointer to a bi structure.
 * @param y Pointer to a bi structure to be compared with x.
 * @return bi_word Result of the comparison (negative, zero, or positive).
 */
bi_word bi_cmp(const bi* x, const bi* y) 
{
	//Return 1 if x is positive and y is negative
	if (x->sign == POSITIVE && y->sign == NEGATIVE)
		return 1;

	//Return -1 if x is negative and y is positive
	if (x->sign == NEGATIVE && y->sign == POSITIVE)
		return -1;

	//Compare the absolute values of x and y
	bi_word ret = bi_cmpABS(x, y);
	
	//If x and y are both positive, return the value of the absolute value comparison
	if (x->sign == POSITIVE)
		return ret;

	//If x and y are both negative, return the opposite value of the absolute value comparison
	else
		return ret * (-1);
}

/**
 * @brief Check if a big integer is equal to zero.
 * 
 * This function determines whether the value of the big integer 'x' is equal
 * to zero. It examines the digit array of 'x' and returns 'true' if all digits
 * are zero, indicating that 'x' represents the value zero. Otherwise, it returns
 * 'false' to indicate that 'x' is not zero.
 * 
 * @param x Pointer to a bi structure representing the big integer to check.
 * @return 'true' if the big integer 'x' is equal to zero, 'false' otherwise.
 */
bool bi_is_zero(const bi* x)
{ 
	// Chek zero in arr [ 0 ]
	return (x->dmax != 1 || x->a[0] !=0 ) ? (false) : (true);
}

/**
 * @brief Check if a big integer is equal to one.
 * 
 * This function determines whether the value of the big integer 'x' is equal
 * to one. It examines the digit array of 'x' and returns 'true' if 'x' represents
 * the value one. Specifically, 'x' is considered to represent one if it has a
 * single non-zero digit (1) at the least significant position and all other
 * digits are zero. Otherwise, it returns 'false' to indicate that 'x' is not one.
 * 
 * @param x Pointer to a bi structure representing the big integer to check.
 * @return 'true' if the big integer 'x' is equal to one, 'false' otherwise.
 */
bool bi_is_one(const bi* x)
{ 
	// Chek zero in arr [ 1 ]
	return (x->dmax != 1 || x->a[0] != 1 ) ? (false) : (true);
}