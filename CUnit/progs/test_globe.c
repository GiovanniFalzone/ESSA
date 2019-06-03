#include <stdio.h>  
#include <string.h>
#include </home/marco/Scrivania/CUnit-2.1-0/CUnit/Headers/Basic.h>
#include </home/marco/Scrivania/CUnit-2.1-0/CUnit/Headers/Automated.h>
#include "globe.h"

int pint[16] = {1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0};

int globe1t[200] = {0, 0, 0, 0, 512, 0, 0, 896, 384, 896, 960, 960, 960, 960, 448, 448, 448, 448, 448, 448, 448,	448, 448, 448, 448,	960, 960, 960, 1984, 1984, 4032, 8160, 16352, 16352, 32736, 32736, 32736, 32736, 32704, 32704, 65504, 65504, 65504, 65504, 32704, 16352, 16320, 48832, 48832, 48832, 16064, 16064, 16064, 48832, 40640, 40512, 40768, 40736, 40864, 36768, 4016, 4016, 3888, 3632, 1584, 1648, 1264, 1264, 240, 504, 504, 504, 504, 248, 248, 248, 248, 248, 120, 120, 120, 120, 48, 176, 144, 144, 32912, 49280, 49152, 49152, 57344, 64128, 64384, 64256, 59904, 59904, 59392, 60416, 60416, 60672, 61184, 61184, 61312, 57216, 57280, 57280, 57024, 52928, 52928, 53184, 57280, 55232, 55232, 55168, 55168, 30592, 30592, 65408, 65408, 65408, 65408, 63360, 42880, 59328, 65472, 65472, 65504, 32736, 65504, 32736, 32736, 65504, 65504, 65472, 65472, 65472, 65472, 65472, 65472, 65472, 65472, 65504, 65504, 65504, 65504, 65504, 65504, 65504, 65504, 6550, 	65504, 65504, 65504, 65504, 65504, 32736, 32704, 32704, 28608, 28608, 28608, 4032, 8128, 8128, 12224, 12160, 12160, 10112, 14208, 5568, 7616, 2496, 2496, 2496, 448, 448, 384, 448, 1920, 1920, 896, 896, 384, 384, 448, 384, 384, 384, 384, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int globe2t[200] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 2, 10, 10, 62, 6206, 15998, 16382, 16382, 8190, 4094, 2044, 2044, 2044, 1020, 1020, 1020, 508, 504, 504, 248, 248, 240, 240, 112, 48, 48, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3, 3, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 15, 15, 127, 255, 511, 1023, 1023, 1023, 1023, 1023, 1023, 511, 511, 511, 255, 255, 126, 14, 14, 205, 197, 225, 97, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3, 3, 7, 7, 3, 1, 0, 0, 0, 0, 0, 5, 5, 12, 13, 9, 27, 19, 17, 24, 412, 908, 908, 908, 905, 971, 962, 450, 488, 480, 488, 488, 1000, 1000, 984, 2032, 2032, 2000, 2000, 912, 896, 256, 0, 0, 0, 0, 0, 0, 128, 2048, 2048, 3072, 1024, 1536, 0, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int wt[100] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6144, 15360, 32382, 32320, 65344, 65344, 65344, 65280, 65280, 65280, 32382, 32330, 15434, 6218, 74, 0, 0, 0, 126, 66, 66, 66, 66, 66, 60, 0, 0, 0, 0, 0, 0, 0, 126, 66, 66, 66, 66, 82, 114, 19968, 18944, 18944, 19070, 19008, 19008, 31296, 64, 0, 0, 0, 126, 66, 66, 8258, 21570, 19010, 23166, 9216, 8192, 20480, 126, 66, 74, 74, 74, 74, 32382, 512, 1024, 2048, 1150, 586, 32330, 74, 74, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void EE_lcd_putc (char a){
  fprintf (stderr, "Door is '%c'", a);
}
void EE_lcd_goto (int a , int b) {
  fprintf (stderr, "I am in (X, Y) = (%d, %d)", a, b);
}



/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int init_suite1(void)
{

  printf ("\nInitializing suite1\n");
  return 0;
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int clean_suite1(void)
{
  printf ("\nCompleting suite1\n");
  return 0;
}

/* testMotor tests the function motor(int).
 * Each element of the array pint is a bit, that corresponds to a port's state, to pilote the motor.
 * To control the right combination, the function motor returns a decimal value, obteined by decimal conversion.
 * This return is matched with the expected value.
 */
void testMotor(void)
{
   int t, res;
   int j = 0;
   for (j=0; j<16; j++) {
   	res = motor(j);
   	t = pint[j]*(2^0) + pint[(j+1)%16]*(2^1) + pint[(j+2)%16]*(2^2) + pint[(j+3)%16]*(2^3);
   	CU_ASSERT(res == t);
   }
}

/* testLed tests the functions led_globe(int) and led_write(int).
 * Each element of arrays globe1t, globe2t and wt represents the state of 16 leds.
 * To control the right combination and the right initialization of indexes, the function led_globe returns the sum of 
 * an element of globe1t and one of globe2t, which are in the same position of the arrays.
 * The function led_write returns the element of the array wt, that corresponds to the passed index.
 * Each result is matched with the expected value.
 */
void testLed(void)
{
  int i, k, z;
  int l1, l2, wr;
  
  for (i=0; i<200; i++) {
    	k = (i+100) % 200;
	z = (i % 100);
	l1 = led_globe(i);
	l2 = led_globe(k);
	wr = led_write(z);
  	CU_ASSERT(l1 == (globe1t[i]+globe2t[i]));
  	CU_ASSERT(l2 == (globe1t[k]+globe2t[k]));
  	CU_ASSERT(wr == wt[z]);
  }
}

/* The main() function for setting up and running the tests.
 * Returns a CUE_SUCCESS on successful running, another
 * CUnit error code on failure.
 */
int main()
{
   CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite("Suite_1", init_suite1, clean_suite1);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   /* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
   if (NULL == CU_add_test(pSuite, "test of motor(int)", testMotor))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if (NULL == CU_add_test(pSuite, "test of led(int)", testLed))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }



   /* Run all tests using the CUnit Basic interface */
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   /*CU_set_output_filename("cunit.txt");
   CU_automated_run_tests();*/
   CU_cleanup_registry();
   return CU_get_error();
}

