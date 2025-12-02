// main.cpp — assert-based tests for DList
// -----------------------------------------------------------------------------
// This file contains minimal test scaffolding (no external framework).
// It uses `assert` and prints progress so you can see which method is being tested.
// Each section lists the edge cases covered.
//
// To build (example):
//     g++ -std=c++17 -O2 -Wall -Wextra -pedantic main.cpp DList.cpp -o dlist_tests
// Or, if DList is header-only in your setup, just compile this file.
//
// Make sure main.cpp is in the same folder as DList.hpp and DListNode.hpp.
// -----------------------------------------------------------------------------

#include <cassert>
#include <iostream>
#include <vector>
#include <initializer_list>
#include <string>
#include "DList.hpp"

static const size_t NOT_FOUND = static_cast<size_t>(-1);

// Helper: check that list contents == expected vector
template <typename ItemType>
static void expect_contents(const DList<ItemType>& L, const std::vector<ItemType>& v) {
    assert(L.length() == v.size());
    for (size_t i = 0; i < v.size(); ++i) {
        // Only rely on non-negative indexing for operator[] (per docs)
        assert(L[static_cast<long>(i)] == v[i]);
    }
}

// Helper: make a list from a std::vector via append (used in many tests)
template <typename ItemType>
static DList<ItemType> make_list(std::initializer_list<ItemType> v) {
    DList <ItemType> L;
    for (auto x : v) L.append(x);
    return L;
}

// -------------------------
// Tests for DList::DList()
// -------------------------
// Edge cases covered:
//  - New list has length() == 0
//  - You can immediately clear() an empty list (should be no-op)
//  - You can append to an empty list
template <typename ItemType>
static void test_ctor_default() {
    std::cout << "[DList::DList()] default constructor\n";
    DList <ItemType> L;
    assert(L.length() == 0);
    L.clear(); // should be no-op
    assert(L.length() == 0);
    L.append(42);
    assert(L.length() == 1);
    assert(L[0] == 42);
}

// --------------------------------
// Tests for DList copy constructor
// --------------------------------
// Edge cases covered:
//  - Copying an empty list
//  - Copying a non-empty list
//  - Deep copy semantics: modifying the original should NOT affect the copy
template <typename ItemType>
static void test_ctor_copy() {
    std::cout << "[DList::DList(const DList&)] copy constructor\n";
    DList<ItemType> empty;
    DList<ItemType> empty_copy(empty);
    assert(empty_copy.length() == 0);

    DList <ItemType> a = make_list({1,2,3});
    DList<ItemType> b(a);                 // copy
    expect_contents(b, {1,2,3}); 

    // mutate original; copy should remain unchanged
    a[1] = 99;
    expect_contents(a, {1,99,3});
    expect_contents(b, {1,2,3});

}

// ---------------------------------
// Tests for DList::operator= (copy)
// ---------------------------------
// Edge cases covered:
//  - Assign empty -> empty
//  - Assign non-empty -> empty
//  - Assign empty -> non-empty
//  - Assign non-empty -> non-empty
//  - Self-assignment is safe
template <typename ItemType>
static void test_assignment() {
    std::cout << "[DList::operator=] assignment\n";

    DList<ItemType> A, B;
    B = A; // empty -> empty
    assert(A.length() == 0 && B.length() == 0);

    A = make_list({7,8,9});
    DList<ItemType> C;          // empty
    C = A;            // non-empty -> empty
    expect_contents(C, {7,8,9});

    DList<ItemType> D = make_list({1,1,1});
    D = D; // self-assign should not corrupt
    expect_contents(D, {1,1,1});

    D = A; // non-empty -> non-empty
    expect_contents(D, {7,8,9});

    A.clear(); // empty -> non-empty
    D = A;
    assert(D.length() == 0);
}

// ------------------------------------
// Tests for DList::append(const T& x)
// ------------------------------------
// Edge cases covered:
//  - Append to empty list
//  - Append multiple times; order preserved
template <typename ItemType>
static void test_append() {
    std::cout << "[DList::append] append behavior\n";
    DList<ItemType> L;
    L.append(10);
    L.append(20);
    L.append(30);
    expect_contents(L, {10,20,30});
}

// ----------------------------------------------------------
// Tests for DList::operator[](long) const / non-const
// ----------------------------------------------------------
// Edge cases covered (per docs, non-negative indexes):
//  - Access first/last/center by index
//  - Non-const overload returns a reference that can be modified
//  - Const overload returns by value and does not allow modification
template <typename ItemType>
static int read_const_index(const DList<ItemType>& L, long pos) {
    // This function forces use of the const operator[]
    return L[pos];
}
template <typename ItemType>
static void test_bracket_ops() {
    std::cout << "[DList::operator[]] const & non-const\n";
    DList<ItemType>L = make_list({5,6,7});
    // read
    assert(L[0] == 5);
    assert(L[1] == 6);
    assert(L[2] == 7);

    // write through non-const ref
    L[1] = 60;
    expect_contents(L, {5,60,7});

    // use const overload
    const DList<ItemType>& CL = L;
    int v = read_const_index(CL, 0);
    assert(v == 5);
}

// --------------------------------------------------
// Tests for DList::insert(long position, const T& x)
// --------------------------------------------------
// Documented behavior (DList.hpp):
//  - Accepts negative or non-negative positions
//  - If position is beyond the end, it appends at the end
//  - If position is beyond the beginning (i.e., very negative), it inserts at the beginning
// Edge cases covered:
//  - Insert at beginning (position = 0)
//  - Insert in the middle (0 < position < length)
//  - Insert beyond end (position >= length)
//  - Insert at a negative index (e.g., position << 0) should clamp to beginning
template <typename ItemType>
static void test_insert() {
    std::cout << "[DList::insert] position clamping and placement\n";

    // base list
    DList<ItemType> L = make_list({10,20,30});

    // beginning
    L.insert(0, 5);
    expect_contents(L, {5,10,20,30});

    // middle
    L.insert(2, 15);
    expect_contents(L, {5,10,15,20,30});

    // beyond end
    long big = static_cast<long>(L.length() + 1000);
    L.insert(big, 40);
    expect_contents(L, {5,10,15,20,30,40});

    // very negative -> beginning
    L.insert(-9999, 0);
    expect_contents(L, {0,5,10,15,20,30,40});
}

// -----------------------
// Tests for DList::clear
// -----------------------
// Edge cases covered:
//  - clear() on non-empty list empties it
//  - clear() on empty list is a no-op
//  - list remains usable after clear()
template <typename ItemType>
static void test_clear() {
    std::cout << "[DList::clear] idempotent and reusable\n";
    DList<ItemType> L = make_list({1,2,3});
    L.clear();
    assert(L.length() == 0);
    L.clear(); // again
    assert(L.length() == 0);

    L.append(9);
    expect_contents(L, {9});
}

// -----------------------------
// Tests for DList::pop(position)
// -----------------------------
// Documented behavior (DList.hpp):
//  - Removes and returns element at index (default position = -1)
//  - (Implementation detail: invalid index is a no-op in the internal _delete())
// Edge cases covered (using only valid indices):
//  - pop() with default (-1) removes the last element
//  - pop(0) removes the first element
//  - pop at a middle position
template <typename ItemType>
static void test_pop() {
    std::cout << "[DList::pop] last, first, middle\n";
    DList<ItemType> L = make_list({10,20,30,40});

    // default: remove last
    ItemType x = L.pop();  // same as L.pop(-1)
    assert(x == 40);
    expect_contents(L, {10,20,30});

    // first
    x = L.pop(0);
    assert(x == 10);
    expect_contents(L, {20,30});

    // middle (which is also last now)
    x = L.pop(1);
    assert(x == 30);
    expect_contents(L, {20});
}

// -------------------------------
// Tests for DList::remove(value)
// -------------------------------
// Documented behavior (DList.hpp):
//  - Removes element from the list (assumed first occurrence, like Python list.remove)
// Edge cases covered:
//  - Remove when multiple copies exist (should remove the first occurrence)
//  - Remove a value not present (should be a no-op)
//  - Remove on a single-element list
template<typename ItemType>
static void test_remove() {
    std::cout << "[DList::remove] first occurrence; missing value is no-op\n";
    DList<ItemType> L = make_list({1,2,3,2,4});
    L.remove(2); // remove first 2
    expect_contents(L, {1,3,2,4});

    // remove missing
    L.remove(999); // should be no-op
    expect_contents(L, {1,3,2,4});

    // single-element removal
    DList<ItemType> S = make_list({7});
    S.remove(7);
    assert(S.length() == 0);
}

// -----------------------------------------
// Tests for DList::index(value, start=0) const
// -----------------------------------------
// Documented behavior (DList.hpp):
//  - Returns non-negative index of x starting at start
//  - Returns -1 if not found (note: return type is size_t; we check for (size_t)-1)
// Edge cases covered:
//  - Find first occurrence from start=0
//  - Find next occurrence using a positive start offset
//  - Not found case returns (size_t)-1
template <typename ItemType>
static void test_index() {
    std::cout << "[DList::index] start offsets and not-found\n";
    DList<ItemType> L = make_list({5,7,5,9});
    assert(L.index(5, 0) == 0);
    assert(L.index(5, 1) == 2);
    assert(L.index(9, 0) == 3);
    assert(L.index(42, 0) == NOT_FOUND);
}

// ------------------------------
// Tests for DList::count(value) 
// ------------------------------
// Edge cases covered:
//  - Count with multiple occurrences
//  - Count when value is absent
template <typename ItemType>
static void test_count() {
    std::cout << "[DList::count] duplicates and zero-count\n";
    DList<ItemType> L = make_list({2,2,2,3,4});
    assert(L.count(2) == 3);
    assert(L.count(3) == 1);
    assert(L.count(99) == 0);
}

// ------------------------------
// Tests for DList::extend(other)
// ------------------------------
// Edge cases covered:
//  - Extend empty by non-empty
//  - Extend non-empty by empty (no change)
//  - Extend non-empty by non-empty; order preserved
template <typename ItemType>
static void test_extend() {
    std::cout << "[DList::extend] concatenation semantics\n";
    DList<ItemType> A;                // empty
    DList<ItemType> B = make_list({1,2,3});
    A.extend(B);
    expect_contents(A, {1,2,3});

    DList<ItemType> C = make_list({10,20});
    DList<ItemType> D;   // empty
    C.extend(D);
    expect_contents(C, {10,20});

    DList<ItemType> E = make_list({7});
    DList<ItemType> F = make_list({8,9});
    E.extend(F);
    expect_contents(E, {7,8,9});

	DList<ItemType> G = make_list({ 1, 2, 3, 4 });
	G.extend(G);
	expect_contents(G, { 1, 2, 3, 4, 1, 2, 3, 4});
}

/* ---------------------------
   std::string focused tests
   --------------------------- */

template <typename ItemType>
static void test_string_ctor_default() {
    std::cout << "[string] ctor_default\n";
    DList<ItemType> L;
    assert(L.length() == 0);
    L.clear();
    assert(L.length() == 0);
    L.append("one");
    assert(L.length() == 1);
    assert(L[0] == "one");
}

template<typename ItemType>
static void test_string_append() {
    std::cout << "[string] append\n";
    DList<ItemType> L;
    L.append("alpha");
    L.append("beta");
    L.append("gamma");
    expect_contents(L, { "alpha","beta","gamma" });
}

template <typename ItemType>
static void test_string_bracket_ops() {
    std::cout << "[string] bracket ops\n";
    DList<ItemType> L= make_list<ItemType>({ "red","green","blue" });
    assert(L[0] == "red");
    assert(L[1] == "green");
    assert(L[2] == "blue");
    L[1] = "emerald";
    expect_contents(L, { "red","emerald","blue" });
    const DList<std::string>& CL = L;
    auto copy = CL[2];
    assert(copy == "blue");
}

template <typename ItemType>
static void test_string_insert() {
    std::cout << "[string] insert\n";
    DList<ItemType> L = make_list<ItemType>({ "b","d","f" });
    // beginning
    L.insert(0, "a");
    expect_contents(L, { "a","b","d","f" });
    // middle
    L.insert(2, "c");
    expect_contents(L, { "a","b","c","d","f" });
    // beyond end
    L.insert(100, "g");
    expect_contents(L, { "a","b","c","d","f","g" });
    // very negative -> beginning
    L.insert(-9999, "AA");
    expect_contents(L, { "AA","a","b","c","d","f","g" });
}

template <typename ItemType>
static void test_string_extend() {
    std::cout << "[string] extend\n";
    DList<ItemType> A = make_list<ItemType>({ "hi" });
    DList<ItemType> B = make_list<ItemType>({ "there","friend" });
    A.extend(B);
    expect_contents(A, { "hi","there","friend" });

    DList<ItemType> C = make_list<ItemType>({ "solo" });
    C.extend(C); // self-extend
    expect_contents(C, { "solo","solo" });
}

template <typename ItemType>
static void test_string_count() {
    std::cout << "[string] count\n";
    DList<ItemType> L = make_list<ItemType>({ "x","y","x","z","x","y" });
    assert(L.count("x") == 3);
    assert(L.count("y") == 2);
    assert(L.count("nope") == 0);
}

/* -----------------------
   double focused tests
   ----------------------- */

template <typename ItemType>
static void test_double_ctor_copy() {
    std::cout << "[double] ctor_copy\n";
    DList<ItemType> empty;
    DList<ItemType> copyEmpty(empty);
    assert(copyEmpty.length() == 0);

    DList<ItemType>A = make_list<ItemType>({ 1.1,2.2,3.3 });
    DList<ItemType> B(A);
    expect_contents(B, { 1.1,2.2,3.3 });

    A[1] = 9.99;
    expect_contents(A, { 1.1,9.99,3.3 });
    expect_contents(B, { 1.1,2.2,3.3 });
}

template <typename ItemType>
static void test_double_assignment() {
    std::cout << "[double] assignment\n";
    DList<ItemType> A, B;
    B = A;
    assert(A.length() == 0 && B.length() == 0);

    A = make_list<ItemType>({ 7.5,8.5,9.5 });
    DList<ItemType> C;
    C = A;
    expect_contents(C, { 7.5,8.5,9.5 });

    DList<ItemType> D = make_list<ItemType>({ 1.0,1.0 });
    D = D; // self
    expect_contents(D, { 1.0,1.0 });

    D = A;
    expect_contents(D, { 7.5,8.5,9.5 });

    A.clear();
    D = A;
    assert(D.length() == 0);
}

template <typename ItemType>
static void test_double_clear() {
    std::cout << "[double] clear\n";
    DList<ItemType> L = make_list<ItemType>({ 3.14,2.71,1.41 });
    L.clear();
    assert(L.length() == 0);
    L.clear();
    assert(L.length() == 0);
    L.append(6.28);
    expect_contents(L, { 6.28 });
}

template <typename ItemType>
static void test_double_pop() {
    std::cout << "[double] pop\n";
    DList<ItemType> L = make_list<ItemType>({ 10.5,20.5,30.5,40.5 });
    ItemType x = L.pop(); // last
    assert(x == 40.5);
    expect_contents(L, { 10.5,20.5,30.5 });
    x = L.pop(0);
    assert(x == 10.5);
    expect_contents(L, { 20.5,30.5 });
    x = L.pop(1);
    assert(x == 30.5);
    expect_contents(L, { 20.5 });
}

template <typename ItemType>
static void test_double_remove() {
    std::cout << "[double] remove\n";
    DList<ItemType> L = make_list<ItemType>({ 1.1,2.2,3.3,2.2,4.4 });
    L.remove(2.2);
    expect_contents(L, { 1.1,3.3,2.2,4.4 });
    L.remove(9.9); // no-op
    expect_contents(L, { 1.1,3.3,2.2,4.4 });
    DList<ItemType> S = make_list<ItemType>({ 7.7 });
    S.remove(7.7);
    assert(S.length() == 0);
}

template <typename ItemType>
static void test_double_index() {
    std::cout << "[double] index\n";
    DList<ItemType> L = make_list<ItemType>({ 5.5,7.7,5.5,9.9 });
    assert(L.index(5.5, 0) == 0);
    assert(L.index(5.5, 1) == 2);
    assert(L.index(9.9, 0) == 3);
    assert(L.index(42.42, 0) == NOT_FOUND);
}

int main() {
    std::cout << "Running DList assert-based tests...\n\n";

    test_ctor_default<int>();
    test_ctor_copy<int>();
    test_assignment<int>();
    test_append<int>();
    test_bracket_ops<int>();
    test_insert<int>();
    test_clear<int>();
    test_pop<int>();
    test_remove<int>();
    test_index<int>();
    test_count<int>();
    test_extend<int>();

    // string tests (first half)
    test_string_ctor_default<std::string>();
    test_string_append<std::string>();
    test_string_bracket_ops<std::string>();
    test_string_insert<std::string>();
    test_string_extend<std::string>();
    test_string_count<std::string>();

    // double tests (second half)
    test_double_ctor_copy<double>();
    test_double_assignment<double>();
    test_double_clear<double>();
    test_double_pop<double>();
    test_double_remove<double>();
    test_double_index<double>();
    std::cout << "\nAll tests passed.\n";
    return 0;
}
