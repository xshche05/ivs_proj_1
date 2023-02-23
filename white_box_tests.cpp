//======== Copyright (c) 2022, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     White Box - test suite
//
// $NoKeywords: $ivs_project_1 $white_box_tests.cpp
// $Author:     Kirill Shchetiniuk <xshche05@stud.fit.vutbr.cz>
// $Date:       $2023-02-20
//============================================================================//
/**
 * @file white_box_tests.cpp
 * @author Kirill Shchetiniuk
 * 
 * @brief Implementace testu hasovaci tabulky.
 */

#include <vector>

#include "gtest/gtest.h"

#include "white_box_code.h"

//============================================================================//
// ** ZDE DOPLNTE TESTY **
//
// Zde doplnte testy hasovaci tabulky, testujte nasledujici:
// 1. Verejne rozhrani hasovaci tabulky
//     - Vsechny funkce z white_box_code.h
//     - Chovani techto metod testuje pro prazdnou i neprazdnou tabulku.
// 2. Chovani tabulky v hranicnich pripadech
//     - Otestujte chovani pri kolizich ruznych klicu se stejnym hashem 
//     - Otestujte chovani pri kolizich hashu namapovane na stejne misto v 
//       indexu
//============================================================================//

class HashMapTest : public ::testing::Test
{
	void SetUp() override
	{
		table = hash_map_ctor();
		ASSERT_NE(table, nullptr);
	}

	void TearDown() override
	{
		hash_map_dtor(table);
		EXPECT_EQ(table->allocated, 0);
	}

	protected:
		hash_map_t* table;
};

// put
TEST_F(HashMapTest, put_one)
{
	EXPECT_EQ(hash_map_put(table, "key", 1), OK);
}

TEST_F(HashMapTest, put_more_than_could_fit)
{
	std::string key;
	size_t allocated = table->allocated;

	for (int i = 0; i < allocated+1; i++)
	{
		key = "key" + std::to_string(i);
		EXPECT_EQ(hash_map_put(table, key.c_str(), 1), OK);
	}
}

TEST_F(HashMapTest, put_existed_key)
{
	ASSERT_EQ(hash_map_put(table, "key", 1), OK);
	EXPECT_EQ(hash_map_put(table, "key", 2), KEY_ALREADY_EXISTS);
}

TEST_F(HashMapTest, put_collision)
{
	ASSERT_EQ(hash_map_put(table, "abc", 1), OK);
	EXPECT_EQ(hash_map_put(table, "cba", 2), OK);
}

// get
TEST_F(HashMapTest, get_existed)
{
	hash_map_put(table, "key", 1);
	int value;
	EXPECT_EQ(hash_map_get(table, "key", &value), OK);
	EXPECT_EQ(value, 1);
}

TEST_F(HashMapTest, get_not_existed)
{
	int value;
	EXPECT_EQ(hash_map_get(table, "key", &value), KEY_ERROR);
}

TEST_F(HashMapTest, get_collision)
{
	hash_map_put(table, "abc", 1);
	hash_map_put(table, "cba", 2);
	hash_map_put(table, "bca", 3);
	hash_map_put(table, "cab", 4);
	hash_map_put(table, "bac", 5);
	hash_map_put(table, "acb", 6);
	int value;
	ASSERT_EQ(hash_map_get(table, "abc", &value), OK);
	EXPECT_EQ(value, 1);
	ASSERT_EQ(hash_map_get(table, "cba", &value), OK);
	EXPECT_EQ(value, 2);
	ASSERT_EQ(hash_map_get(table, "bca", &value), OK);
	EXPECT_EQ(value, 3);
	ASSERT_EQ(hash_map_get(table, "cab", &value), OK);
	EXPECT_EQ(value, 4);
	ASSERT_EQ(hash_map_get(table, "bac", &value), OK);
	EXPECT_EQ(value, 5);
	ASSERT_EQ(hash_map_get(table, "acb", &value), OK);
	EXPECT_EQ(value, 6);
}

//pop
TEST_F(HashMapTest, pop_existed_first)
{
	hash_map_put(table, "key", 1);
	int value;
	EXPECT_EQ(hash_map_pop(table, "key", &value), OK);
	EXPECT_EQ(value, 1);
}

TEST_F(HashMapTest, pop_existed_last)
{
	hash_map_put(table, "key", 1);
	hash_map_put(table, "key2", 2);
	int value;
	EXPECT_EQ(hash_map_pop(table, "key2", &value), OK);
	EXPECT_EQ(value, 2);
}

TEST_F(HashMapTest, pop_existed_not_first_or_last)
{
	hash_map_put(table, "key", 1);
	hash_map_put(table, "key2", 2);
	hash_map_put(table, "key3", 3);
	int value;
	EXPECT_EQ(hash_map_pop(table, "key2", &value), OK);
	EXPECT_EQ(value, 2);
}

TEST_F(HashMapTest, pop_not_existed)
{
	int value;
	EXPECT_EQ(hash_map_pop(table, "key", &value), KEY_ERROR);
}

TEST_F(HashMapTest, used_after_pop)
{
	int value;
	hash_map_put(table, "key", 1);
	hash_map_pop(table, "key", &value);
	EXPECT_EQ(table->used, 0);
}

// remove
TEST_F(HashMapTest, remove_existed)
{
	hash_map_put(table, "key", 1);
	EXPECT_EQ(hash_map_remove(table, "key"), OK);
}

TEST_F(HashMapTest, remove_not_existed)
{
	EXPECT_EQ(hash_map_remove(table, "key"), KEY_ERROR);
}

TEST_F(HashMapTest, used_after_remove)
{
	hash_map_put(table, "key", 1);
	hash_map_remove(table, "key");
	EXPECT_EQ(table->used, 0);
}

// contains
TEST_F(HashMapTest, contains_existed)
{
	hash_map_put(table, "key", 1);
	EXPECT_EQ(hash_map_contains(table, "key"), true);
}

TEST_F(HashMapTest, contains_not_existed)
{
	EXPECT_EQ(hash_map_contains(table, "key"), false);
}

// size
TEST_F(HashMapTest, size)
{
	EXPECT_EQ(hash_map_size(table), table->used);
}

// capacity
TEST_F(HashMapTest, capacity)
{
	EXPECT_EQ(hash_map_capacity(table), table->allocated);
}

// reserve
TEST_F(HashMapTest, reserve_less_used)
{
	hash_map_put(table, "key", 1);
	hash_map_put(table, "key2", 2);
	EXPECT_EQ(hash_map_reserve(table, table->used-1), VALUE_ERROR);
}

TEST_F(HashMapTest, reserve_more_allocated)
{
	hash_map_put(table, "key", 1);
	hash_map_put(table, "key2", 2);
	EXPECT_EQ(hash_map_reserve(table, table->allocated+1), OK);
}

TEST_F(HashMapTest, reserve_same_allocated)
{
	hash_map_put(table, "key", 1);
	hash_map_put(table, "key2", 2);
	EXPECT_EQ(hash_map_reserve(table, table->allocated), OK);
}

TEST_F(HashMapTest, reserve_more_than_sys_possible)
{
	EXPECT_EQ(hash_map_reserve(table, SIZE_MAX), MEMORY_ERROR);
}

// clear
TEST_F(HashMapTest, clear)
{
	hash_map_put(table, "key", 1);
	hash_map_clear(table);
	EXPECT_EQ(table->used, 0);
}

/*** Konec souboru white_box_tests.cpp ***/
