/*
 * test_runner.c - struct2json automated test suite
 *
 * Tests cover: basic serialization, deserialization, round-trip consistency,
 * NULL handling, nested structs, array elements, and edge cases.
 *
 * Build:  see CMakeLists.txt or Makefile.standalone
 * Run:    ./tests/test_s2j
 */

#include "unity.h"
#include "s2j.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* ================================================================
 * Test structures
 * ================================================================ */

typedef struct {
    double lat;
    double lon;
    char name[16];
} Location;

typedef struct {
    uint8_t id;
    char name[10];
    double weight;
    int score[4];
    Location location;
} Person;

/* ================================================================
 * struct <-> JSON conversion functions
 * ================================================================ */

static cJSON *struct_to_json_Location(void *struct_obj)
{
    Location *s = (Location *)struct_obj;

    s2j_create_json_obj(json);
    s2j_json_set_basic_element(json, s, double, lat);
    s2j_json_set_basic_element(json, s, double, lon);
    s2j_json_set_basic_element(json, s, string, name);
    return json;
}

static void *json_to_struct_Location(cJSON *json_obj)
{
    s2j_create_struct_obj(s, Location);
    s2j_struct_get_basic_element(s, json_obj, double, lat);
    s2j_struct_get_basic_element(s, json_obj, double, lon);
    s2j_struct_get_basic_element(s, json_obj, string, name);
    return s;
}

static cJSON *struct_to_json_Person(void *struct_obj)
{
    Person *s = (Person *)struct_obj;

    s2j_create_json_obj(json);
    s2j_json_set_basic_element(json, s, int, id);
    s2j_json_set_basic_element(json, s, double, weight);
    s2j_json_set_array_element(json, s, int, score, 4);
    s2j_json_set_basic_element(json, s, string, name);
    s2j_json_set_struct_element(json_loc, json, s_loc, s, Location, location);
    s2j_json_set_basic_element(json_loc, s_loc, double, lat);
    s2j_json_set_basic_element(json_loc, s_loc, double, lon);
    s2j_json_set_basic_element(json_loc, s_loc, string, name);
    return json;
}

static void *json_to_struct_Person(cJSON *json_obj)
{
    s2j_create_struct_obj(s, Person);
    s2j_struct_get_basic_element(s, json_obj, int, id);
    s2j_struct_get_basic_element(s, json_obj, double, weight);
    s2j_struct_get_array_element(s, json_obj, int, score);
    s2j_struct_get_basic_element(s, json_obj, string, name);
    s2j_struct_get_struct_element(s_loc, s, json_loc, json_obj, Location, location);
    s2j_struct_get_basic_element(s_loc, json_loc, double, lat);
    s2j_struct_get_basic_element(s_loc, json_loc, double, lon);
    s2j_struct_get_basic_element(s_loc, json_loc, string, name);
    return s;
}

/* ================================================================
 * Test cases
 * ================================================================ */

/* ---- JSON object lifecycle ---- */

static void test_create_and_delete_json_obj(void)
{
    s2j_create_json_obj(json);
    TEST_ASSERT_NOT_NULL(json);
    s2j_delete_json_obj(json);
}

/* ---- Basic serialization ---- */

static void test_struct_to_json_basic_fields(void)
{
    Location loc = { .lat = 39.9, .lon = 116.4, .name = "Beijing" };

    cJSON *json = struct_to_json_Location(&loc);
    TEST_ASSERT_NOT_NULL(json);

    cJSON *lat = cJSON_GetObjectItem(json, "lat");
    TEST_ASSERT_NOT_NULL(lat);
    TEST_ASSERT_EQUAL_DOUBLE(39.9, lat->valuedouble, 0.01);

    cJSON *lon = cJSON_GetObjectItem(json, "lon");
    TEST_ASSERT_NOT_NULL(lon);
    TEST_ASSERT_EQUAL_DOUBLE(116.4, lon->valuedouble, 0.01);

    cJSON *name = cJSON_GetObjectItem(json, "name");
    TEST_ASSERT_NOT_NULL(name);
    TEST_ASSERT_EQUAL_STRING("Beijing", name->valuestring);

    s2j_delete_json_obj(json);
}

/* ---- Basic deserialization ---- */

static void test_json_to_struct_basic_fields(void)
{
    const char *json_str =
        "{\"lat\": 31.2, \"lon\": 121.5, \"name\": \"Shanghai\"}";
    cJSON *json = cJSON_Parse(json_str);
    TEST_ASSERT_NOT_NULL(json);

    Location *loc = (Location *)json_to_struct_Location(json);
    TEST_ASSERT_NOT_NULL(loc);
    TEST_ASSERT_EQUAL_DOUBLE(31.2, loc->lat, 0.01);
    TEST_ASSERT_EQUAL_DOUBLE(121.5, loc->lon, 0.01);
    TEST_ASSERT_EQUAL_STRING("Shanghai", loc->name);

    s2j_delete_struct_obj(loc);
    cJSON_Delete(json);
}

/* ---- Round-trip consistency ---- */

static void test_roundtrip_person(void)
{
    Person orig;
    memset(&orig, 0, sizeof(Person));
    orig.id     = 42;
    orig.weight = 65.5;
    strncpy(orig.name, "Alice", sizeof(orig.name) - 1);
    orig.score[0] = 90;
    orig.score[1] = 85;
    orig.score[2] = 92;
    orig.score[3] = 78;
    orig.location.lat = 40.7;
    orig.location.lon = -74.0;
    strncpy(orig.location.name, "NewYork",
            sizeof(orig.location.name) - 1);

    /* struct -> JSON -> struct */
    cJSON *json = struct_to_json_Person(&orig);
    TEST_ASSERT_NOT_NULL(json);

    Person *result = (Person *)json_to_struct_Person(json);
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_MEMORY(&orig, result, sizeof(Person));

    s2j_delete_struct_obj(result);
    s2j_delete_json_obj(json);
}

static void test_double_roundtrip_consistency(void)
{
    Person orig;
    memset(&orig, 0, sizeof(Person));
    orig.id     = 7;
    orig.weight = 55.0;
    strncpy(orig.name, "Bob", sizeof(orig.name) - 1);
    orig.score[0] = 100;
    orig.score[1] = 99;
    orig.score[2] = 98;
    orig.score[3] = 97;
    orig.location.lat = 35.7;
    orig.location.lon = 139.7;
    strncpy(orig.location.name, "Tokyo",
            sizeof(orig.location.name) - 1);

    /* First trip: struct -> JSON -> struct */
    cJSON *json1 = struct_to_json_Person(&orig);
    Person *mid  = (Person *)json_to_struct_Person(json1);
    TEST_ASSERT_NOT_NULL(mid);

    /* Second trip: struct -> JSON -> struct */
    cJSON *json2 = struct_to_json_Person(mid);
    Person *result = (Person *)json_to_struct_Person(json2);
    TEST_ASSERT_NOT_NULL(result);

    TEST_ASSERT_EQUAL_MEMORY(mid, result, sizeof(Person));

    s2j_delete_struct_obj(mid);
    s2j_delete_struct_obj(result);
    s2j_delete_json_obj(json1);
    s2j_delete_json_obj(json2);
}

/* ---- NULL handling ---- */

static void test_null_json_returns_zeroed_struct(void)
{
    /* _ex API with default values should handle NULL JSON gracefully */
    s2j_create_struct_obj(s, Location);
    TEST_ASSERT_NOT_NULL(s);

    /* With NULL json, basic macros should skip assignment (fields stay 0) */
    s2j_struct_get_basic_element(s, NULL, double, lat);
    TEST_ASSERT_EQUAL_DOUBLE(0.0, s->lat, 0.001);

    s2j_struct_get_basic_element(s, NULL, string, name);
    TEST_ASSERT_EQUAL_STRING("", s->name);

    s2j_delete_struct_obj(s);
}

/* ---- Nested struct ---- */

static void test_nested_struct_roundtrip(void)
{
    Person orig;
    memset(&orig, 0, sizeof(Person));
    orig.id     = 1;
    orig.weight = 70.0;
    strncpy(orig.name, "Test", sizeof(orig.name) - 1);
    orig.score[0] = 1;
    orig.score[1] = 2;
    orig.score[2] = 3;
    orig.score[3] = 4;
    orig.location.lat = 51.5;
    orig.location.lon = -0.1;
    strncpy(orig.location.name, "London",
            sizeof(orig.location.name) - 1);

    cJSON *json = struct_to_json_Person(&orig);
    TEST_ASSERT_NOT_NULL(json);

    /* Verify nested JSON object exists */
    cJSON *loc_json = cJSON_GetObjectItem(json, "location");
    TEST_ASSERT_NOT_NULL(loc_json);

    cJSON *lat_json = cJSON_GetObjectItem(loc_json, "lat");
    TEST_ASSERT_NOT_NULL(lat_json);
    TEST_ASSERT_EQUAL_DOUBLE(51.5, lat_json->valuedouble, 0.01);

    /* Full round-trip */
    Person *result = (Person *)json_to_struct_Person(json);
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_MEMORY(&orig, result, sizeof(Person));

    s2j_delete_struct_obj(result);
    s2j_delete_json_obj(json);
}

/* ---- Array elements ---- */

static void test_array_elements(void)
{
    Person orig;
    memset(&orig, 0, sizeof(Person));
    orig.id     = 99;
    orig.weight = 80.0;
    strncpy(orig.name, "ArrayTst", sizeof(orig.name) - 1);
    orig.score[0] = 10;
    orig.score[1] = 20;
    orig.score[2] = 30;
    orig.score[3] = 40;
    orig.location.lat = 0.0;
    orig.location.lon = 0.0;
    strncpy(orig.location.name, "Null",
            sizeof(orig.location.name) - 1);

    cJSON *json = struct_to_json_Person(&orig);
    TEST_ASSERT_NOT_NULL(json);

    /* Verify JSON array */
    cJSON *score_array = cJSON_GetObjectItem(json, "score");
    TEST_ASSERT_NOT_NULL(score_array);
    TEST_ASSERT_EQUAL_INT(4, cJSON_GetArraySize(score_array));

    cJSON *s0 = cJSON_GetArrayItem(score_array, 0);
    TEST_ASSERT_NOT_NULL(s0);
    TEST_ASSERT_EQUAL_INT(10, s0->valueint);

    cJSON *s3 = cJSON_GetArrayItem(score_array, 3);
    TEST_ASSERT_NOT_NULL(s3);
    TEST_ASSERT_EQUAL_INT(40, s3->valueint);

    /* Round-trip */
    Person *result = (Person *)json_to_struct_Person(json);
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_INT(10, result->score[0]);
    TEST_ASSERT_EQUAL_INT(20, result->score[1]);
    TEST_ASSERT_EQUAL_INT(30, result->score[2]);
    TEST_ASSERT_EQUAL_INT(40, result->score[3]);

    s2j_delete_struct_obj(result);
    s2j_delete_json_obj(json);
}

/* ================================================================
 * Main
 * ================================================================ */

int main(void)
{
    printf("struct2json test suite\n");
    printf("========================================\n\n");

    RUN_TEST(test_create_and_delete_json_obj);
    RUN_TEST(test_struct_to_json_basic_fields);
    RUN_TEST(test_json_to_struct_basic_fields);
    RUN_TEST(test_roundtrip_person);
    RUN_TEST(test_double_roundtrip_consistency);
    RUN_TEST(test_null_json_returns_zeroed_struct);
    RUN_TEST(test_nested_struct_roundtrip);
    RUN_TEST(test_array_elements);

    return unity_end();
}
