#include "stack.h"
#include <gtest.h>

TEST(TStack, can_create_stack_with_positive_length)
{
  ASSERT_NO_THROW(TStack<int> st(5));
}

TEST(TStack, throws_create_stack_with_invalid_length)
{
	ASSERT_ANY_THROW(TStack<int> st(-1));
}

TEST(TStack, can_push_element_to_stack)
{
	TStack<int> st(5);
	ASSERT_NO_THROW(st.push(42));
}

TEST(TStack, can_read_top)
{
	TStack<int> st(5);
	st.push(42);
	ASSERT_EQ(st.get_top(), 42);
}

TEST(TStack, can_pop_top)
{
	TStack<int> st(5);
	st.push(42);
	st.push(99);
	ASSERT_EQ(st.get_top(), 99);
	st.pop();
	ASSERT_EQ(st.get_top(), 42);
}

TEST(TStack, can_auto_resize)
{
	TStack<int> st(2);
	for (int i = 0; i < 16;i++) {
		st.push(i);
	}
	for (int i = 0; i < 16;i++) {
		ASSERT_EQ(st.get_top(), 16-1-i);
		st.pop();
	}
}

TEST(TStack, throws_auto_resize_past_limit)
{
	TStack<int> st(MaxStackSize-1);
	for (int i = 1; i <= MaxStackSize-1;i++) {
		st.push(i);
	}
	ASSERT_ANY_THROW(st.push(100));
}

TEST(TStack, nothrow_random_actions)
{
	TStack<int> st(10);
	for (int i = 1; i <= 10;i++) {
		st.push(i);
		if (i % 2 == 0)
			st.pop();
	}
}

TEST(TStack, throws_pop_empty)
{
	TStack<int> st(1);
	st.push(1);
	st.pop();
	
	ASSERT_ANY_THROW(st.pop());
}