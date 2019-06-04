#include <stdio.h>	
#include <string.h>
#include <CUnit/CUnit.h>
#include <CUnit/Automated.h> // Automated interface with xml output.
#include <CUnit/Basic.h>		 // Basic interface with console output.
#include "ESSTA_JSON_lib.h"

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


	// check &= set_room_ID(str, &my_room);
	// check &= set_room_Eco(str, &my_room);
	// check &= set_room_Temperature(str, &my_room);
	// check &= set_room_Humidity(str, &my_room);
	// check &= set_room_Valve(str, &my_room);
	// check_message(char* str);
// check JSON compliance
// if(len>=MSG_LEN)
// 	return 3;	// message corrupted

// if(count_start_graph > count_end_graph)	// message incomplete
// 	return 2;

// if(count_start_graph==count_end_graph && count_start_quad==count_end_quad && (count_quotes%2)==0)	// message correct
// 	return 1;
void testJSON_check_message(void) {
	int i;
	char msg[MSG_LEN];
	// missing odd number of quotes
	memset(msg, '?', MSG_LEN);
	sprintf(msg,"{Id\":\"02\",\"Eco\":\"0\",\"sens\":[{\"Nm\":\"Tmp\",\"Val\":\"00.00\",\"Fmt\":\"C\"},{\"Nm\":\"Hum\",\"Val\":\"12.12\",\"Fmt\":\"%%\"}],\"acts\":[{\"Nm\":\"Vlv\",\"Val\":\"000\",\"Fmt\":\"%%\"}]})\n");
	CU_ASSERT(3 == check_message(msg));

	// missing odd number of graph
	memset(msg, '?', MSG_LEN);
	sprintf(msg,"\"Id\":\"02\",\"Eco\":\"0\",\"sens\":[{\"Nm\":\"Tmp\",\"Val\":\"00.00\",\"Fmt\":\"C\"},{\"Nm\":\"Hum\",\"Val\":\"12.12\",\"Fmt\":\"%%\"}],\"acts\":[{\"Nm\":\"Vlv\",\"Val\":\"000\",\"Fmt\":\"%%\"}]})\n");
	CU_ASSERT(3 == check_message(msg));

	// missing odd number of quad
	memset(msg, '?', MSG_LEN);
	sprintf(msg,"{\"Id\":\"02\",\"Eco\":\"0\",\"sens\":{\"Nm\":\"Tmp\",\"Val\":\"00.00\",\"Fmt\":\"C\"},{\"Nm\":\"Hum\",\"Val\":\"12.12\",\"Fmt\":\"%%\"}],\"acts\":[{\"Nm\":\"Vlv\",\"Val\":\"000\",\"Fmt\":\"%%\"}]})\n");
	CU_ASSERT(3 == check_message(msg));

	// wrong Id field
	memset(msg, '?', MSG_LEN);
	sprintf(msg,"{\"??\":\"02\",\"Eco\":\"0\",\"sens\":{\"Nm\":\"Tmp\",\"Val\":\"00.00\",\"Fmt\":\"C\"},{\"Nm\":\"Hum\",\"Val\":\"12.12\",\"Fmt\":\"%%\"}],\"acts\":[{\"Nm\":\"Vlv\",\"Val\":\"000\",\"Fmt\":\"%%\"}]})\n");
	CU_ASSERT(3 == check_message(msg));

	// wrong Eco field
	memset(msg, '?', MSG_LEN);
	sprintf(msg,"{\"Id\":\"02\",\"???\":\"0\",\"sens\":{\"Nm\":\"Tmp\",\"Val\":\"00.00\",\"Fmt\":\"C\"},{\"Nm\":\"Hum\",\"Val\":\"12.12\",\"Fmt\":\"%%\"}],\"acts\":[{\"Nm\":\"Vlv\",\"Val\":\"000\",\"Fmt\":\"%%\"}]})\n");
	CU_ASSERT(3 == check_message(msg));

	// wrong Nm field
	memset(msg, '?', MSG_LEN);
	sprintf(msg,"{\"Id\":\"02\",\"Eco\":\"0\",\"sens\":{\"??\":\"Tmp\",\"Val\":\"00.00\",\"Fmt\":\"C\"},{\"Nm\":\"Hum\",\"Val\":\"12.12\",\"Fmt\":\"%%\"}],\"acts\":[{\"Nm\":\"Vlv\",\"Val\":\"000\",\"Fmt\":\"%%\"}]})\n");
	CU_ASSERT(3 == check_message(msg));

	// wrong Tmp field
	memset(msg, '?', MSG_LEN);
	sprintf(msg,"{\"Id\":\"02\",\"Eco\":\"0\",\"sens\":{\"Nm\":\"???\",\"Val\":\"00.00\",\"Fmt\":\"C\"},{\"Nm\":\"Hum\",\"Val\":\"12.12\",\"Fmt\":\"%%\"}],\"acts\":[{\"Nm\":\"Vlv\",\"Val\":\"000\",\"Fmt\":\"%%\"}]})\n");
	CU_ASSERT(3 == check_message(msg));

	// wrong Val field
	memset(msg, '?', MSG_LEN);
	sprintf(msg,"{\"Id\":\"02\",\"Eco\":\"0\",\"sens\":{\"Nm\":\"Tmp\",\"??\":\"00.00\",\"Fmt\":\"C\"},{\"Nm\":\"Hum\",\"Val\":\"12.12\",\"Fmt\":\"%%\"}],\"acts\":[{\"Nm\":\"Vlv\",\"Val\":\"000\",\"Fmt\":\"%%\"}]})\n");
	CU_ASSERT(3 == check_message(msg));

	// wrong Fmt field
	memset(msg, '?', MSG_LEN);
	sprintf(msg,"{\"Id\":\"02\",\"Eco\":\"0\",\"sens\":{\"Nm\":\"Tmp\",\"Val\":\"00.00\",\"???\":\"C\"},{\"Nm\":\"Hum\",\"Val\":\"12.12\",\"Fmt\":\"%%\"}],\"acts\":[{\"Nm\":\"Vlv\",\"Val\":\"000\",\"Fmt\":\"%%\"}]})\n");
	CU_ASSERT(3 == check_message(msg));

	// wrong Hum field
	memset(msg, '?', MSG_LEN);
	sprintf(msg,"{\"Id\":\"02\",\"Eco\":\"0\",\"sens\":{\"Nm\":\"Tmp\",\"Val\":\"00.00\",\"Fmt\":\"C\"},{\"Nm\":\"???\",\"Val\":\"12.12\",\"Fmt\":\"%%\"}],\"acts\":[{\"Nm\":\"Vlv\",\"Val\":\"000\",\"Fmt\":\"%%\"}]})\n");
	CU_ASSERT(3 == check_message(msg));

	// wrong Vlv field
	memset(msg, '?', MSG_LEN);
	sprintf(msg,"{\"Id\":\"02\",\"Eco\":\"0\",\"sens\":{\"Nm\":\"Tmp\",\"Val\":\"00.00\",\"Fmt\":\"C\"},{\"Nm\":\"Hum\",\"Val\":\"12.12\",\"Fmt\":\"%%\"}],\"acts\":[{\"Nm\":\"???\",\"Val\":\"000\",\"Fmt\":\"%%\"}]})\n");
	CU_ASSERT(3 == check_message(msg));
}

void testJSON_eco(void) {
	int i;
	// min:0, max:1
	int eco_err_test[3] = {-1, 2, 10};
	char msg[MSG_LEN];
	struct room_struct tmp;
	 for (i=0; i<3; i++) {
		sprintf(msg,"{\"Id\":\"02\",\"Eco\":\"0\",\"sens\":[{\"Nm\":\"Tmp\",\"Val\":\"00.00\",\"Fmt\":\"C\"},{\"Nm\":\"Hum\",\"Val\":\"12.12\",\"Fmt\":\"%%\"}],\"acts\":[{\"Nm\":\"Vlv\",\"Val\":\"%i\",\"Fmt\":\"%%\"}]})\n", eco_err_test[i]);
		tmp = JSON_to_room_struct(msg);
	 	CU_ASSERT(255 == tmp.id);
	 }
}
void testJSON_valve(void) {
	int i;
	// min:0, max:100
	int valve_err_test[3] = {-1, 101, 1000};
	char msg[MSG_LEN];
	struct room_struct tmp;
	 for (i=0; i<3; i++) {
		sprintf(msg,"{\"Id\":\"02\",\"Eco\":\"1\",\"sens\":[{\"Nm\":\"Tmp\",\"Val\":\"00.00\",\"Fmt\":\"C\"},{\"Nm\":\"Hum\",\"Val\":\"12.12\",\"Fmt\":\"%%\"}],\"acts\":[{\"Nm\":\"Vlv\",\"Val\":\"%i\",\"Fmt\":\"%%\"}]})\n", valve_err_test[i]);
		tmp = JSON_to_room_struct(msg);
	 	CU_ASSERT(255 == tmp.id);
	 }
}
void testJSON_ID(void) {
	int i;
	// min:01, max:02
	int id_err_test[4] = {0, 3, -1, 10};
	char msg[256];
	struct room_struct tmp;
	for (i=0; i<4; i++) {
		sprintf(msg,"{\"Id\":\"%i\",\"Eco\":\"1\",\"sens\":[{\"Nm\":\"Tmp\",\"Val\":\"12.12\",\"Fmt\":\"C\"},{\"Nm\":\"Hum\",\"Val\":\"62.34\",\"Fmt\":\"%%\"}],\"acts\":[{\"Nm\":\"Vlv\",\"Val\":\"057\",\"Fmt\":\"%%\"}]})\n", id_err_test[i]);
		tmp = JSON_to_room_struct(msg);
		CU_ASSERT(255 == tmp.id);
	}
}
void testJSON_temperature(void) {
	int i;
	// min:15, max:30
	float temp_err_test[3] = {0.00, 50.00, -1.00};
	char msg[MSG_LEN];
	struct room_struct tmp;
	for (i=0; i<3; i++) {
		sprintf(msg,"{\"Id\":\"02\",\"Eco\":\"1\",\"sens\":[{\"Nm\":\"Tmp\",\"Val\":\"%2.2f\",\"Fmt\":\"C\"},{\"Nm\":\"Hum\",\"Val\":\"62.34\",\"Fmt\":\"%%\"}],\"acts\":[{\"Nm\":\"Vlv\",\"Val\":\"057\",\"Fmt\":\"%%\"}]})\n", temp_err_test[i]);
		tmp = JSON_to_room_struct(msg);
		CU_ASSERT(255 == tmp.id);
	}
	sprintf(msg,"{\"Id\":\"02\",\"Eco\":\"1\",\"sens\":[{\"Nm\":\"Tmp\",\"Val\":\"123.123\",\"Fmt\":\"C\"},{\"Nm\":\"Hum\",\"Val\":\"12.12\",\"Fmt\":\"%%\"}],\"acts\":[{\"Nm\":\"Vlv\",\"Val\":\"057\",\"Fmt\":\"%%\"}]})\n");
	tmp = JSON_to_room_struct(msg);
	CU_ASSERT(255 == tmp.id);
}
void testJSON_humidity(void) {
	int i;
	// min:0, max:100
	float hum_err_test[3] = {-1, 101, 1000};
	char msg[MSG_LEN];
	struct room_struct tmp;
	for (i=0; i<3; i++) {
		sprintf(msg,"{\"Id\":\"02\",\"Eco\":\"1\",\"sens\":[{\"Nm\":\"Tmp\",\"Val\":\"00.00\",\"Fmt\":\"C\"},{\"Nm\":\"Hum\",\"Val\":\"%2.2f\",\"Fmt\":\"%%\"}],\"acts\":[{\"Nm\":\"Vlv\",\"Val\":\"057\",\"Fmt\":\"%%\"}]})\n", hum_err_test[i]);
		tmp = JSON_to_room_struct(msg);
		CU_ASSERT(255 == tmp.id);
	}
	sprintf(msg,"{\"Id\":\"02\",\"Eco\":\"1\",\"sens\":[{\"Nm\":\"Tmp\",\"Val\":\"00.00\",\"Fmt\":\"C\"},{\"Nm\":\"Hum\",\"Val\":\"123.123\",\"Fmt\":\"%%\"}],\"acts\":[{\"Nm\":\"Vlv\",\"Val\":\"057\",\"Fmt\":\"%%\"}]})\n");
	tmp = JSON_to_room_struct(msg);
	CU_ASSERT(255 == tmp.id);
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
	 if (NULL == CU_add_test(pSuite, "testJSON_check_message", testJSON_check_message))
	 {
			CU_cleanup_registry();
			return CU_get_error();
	 }

	 if (NULL == CU_add_test(pSuite, "testJSON_ID", testJSON_ID))
	 {
			CU_cleanup_registry();
			return CU_get_error();
	 }

	 if (NULL == CU_add_test(pSuite, "testJSON_eco", testJSON_eco))
	 {
			CU_cleanup_registry();
			return CU_get_error();
	 }

	 if (NULL == CU_add_test(pSuite, "testJSON_temperature", testJSON_temperature))
	 {
			CU_cleanup_registry();
			return CU_get_error();
	 }

	 if (NULL == CU_add_test(pSuite, "testJSON_humidity", testJSON_humidity))
	 {
			CU_cleanup_registry();
			return CU_get_error();
	 }

	 if (NULL == CU_add_test(pSuite, "testJSON_valve", testJSON_valve))
	 {
			CU_cleanup_registry();
			return CU_get_error();
	 }


	 /* Run all tests using the CUnit Basic interface */
	 CU_basic_set_mode(CU_BRM_VERBOSE);
	 CU_basic_run_tests();
	 CU_set_output_filename("cunit.txt");
	 CU_automated_run_tests();
	 CU_cleanup_registry();
	 return CU_get_error();
}

