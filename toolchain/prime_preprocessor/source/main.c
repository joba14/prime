
/**
 * @file main.c
 * 
 * @copyright This file is part of the "Prime" project and is distributed under
 * "Prime GPLv1" license.
 * 
 * @author joba14
 * 
 * @date 2023-09-30
 */

#include <stdio.h>

signed int main(
	const signed int argc,
	const char** const argv);

signed int main(
	const signed int argc,
	const char** const argv)
{
	(void)argc;
	(void)argv;
	fprintf(stdout, "hello, prime_preprocessor!\n");
	return 0;
}
