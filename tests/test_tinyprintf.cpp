// Basic unit tests for tinyprintf (x_printf).
//
// x_printf emits through the platform hook x_write(). For tests we provide our
// own x_write that captures everything into a std::string, so we can assert on
// the exact bytes produced.

#include <gtest/gtest.h>

#include <climits>
#include <cstdarg>
#include <cstdio>
#include <string>

extern "C" {
#include "tinyprintf.h"
}

static std::string g_out;

// Test backend for x_write: capture instead of writing to stdout.
extern "C" int x_write(char const *p, int n)
{
	g_out.append(p, static_cast<size_t>(n));
	return n;
}

namespace {

// Run x_printf, return the captured output. Reset the buffer each call.
template <class... Args>
std::string cap(char const *fmt, Args... args)
{
	g_out.clear();
	x_printf(fmt, args...);
	return g_out;
}

// The value x_printf returns must equal the number of bytes it emitted.
template <class... Args>
int ret_of(char const *fmt, Args... args)
{
	g_out.clear();
	return x_printf(fmt, args...);
}

// Reference implementation (glibc) for cross-checking.
std::string ref(char const *fmt, ...)
{
	char buf[512];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);
	return std::string(buf);
}

} // namespace

TEST(TinyPrintf, PlainText)
{
	EXPECT_EQ(cap("hello world"), "hello world");
	EXPECT_EQ(cap(""), "");
}

TEST(TinyPrintf, Decimal)
{
	EXPECT_EQ(cap("%d", 0), "0");
	EXPECT_EQ(cap("%d", 42), "42");
	EXPECT_EQ(cap("%d", -42), "-42");
	EXPECT_EQ(cap("%d", INT_MAX), "2147483647");
}

TEST(TinyPrintf, DecimalIntMinNoUB)
{
	// -(INT_MIN) would overflow if computed as signed; must still print right.
	EXPECT_EQ(cap("%d", INT_MIN), "-2147483648");
	EXPECT_EQ(cap("%d", INT_MIN), ref("%d", INT_MIN));
}

TEST(TinyPrintf, Unsigned)
{
	EXPECT_EQ(cap("%u", 0u), "0");
	EXPECT_EQ(cap("%u", 42u), "42");
	// Value that is negative as signed but large as unsigned.
	EXPECT_EQ(cap("%u", (unsigned)-1), "4294967295");
	EXPECT_EQ(cap("%u", 4294967295u), ref("%u", 4294967295u));
}

TEST(TinyPrintf, UnsignedWithWidthAndZeroPad)
{
	EXPECT_EQ(cap("%8u", 42u), "      42");
	EXPECT_EQ(cap("%08u", 42u), "00000042");
	EXPECT_EQ(cap("%08u", 42u), ref("%08u", 42u));
}

TEST(TinyPrintf, Hex)
{
	EXPECT_EQ(cap("%x", 0xabc), "abc");
	EXPECT_EQ(cap("%X", 0xDEADBEEF), "DEADBEEF");
	EXPECT_EQ(cap("%08x", 0xab), "000000ab");
	EXPECT_EQ(cap("%08x", 0x1234abcd), ref("%08x", 0x1234abcd));
}

TEST(TinyPrintf, WidthSpacePadding)
{
	EXPECT_EQ(cap("%5d", 42), "   42");
	EXPECT_EQ(cap("%5d", -42), "  -42");   // sign must not steal a pad slot
	EXPECT_EQ(cap("%5d", -42), ref("%5d", -42));
}

TEST(TinyPrintf, WidthZeroPaddingSigned)
{
	EXPECT_EQ(cap("%05d", 42), "00042");
	EXPECT_EQ(cap("%05d", -42), "-0042"); // sign before zeros
	EXPECT_EQ(cap("%05d", -42), ref("%05d", -42));
}

TEST(TinyPrintf, PlusFlag)
{
	EXPECT_EQ(cap("%+d", 42), "+42");
	EXPECT_EQ(cap("%+5d", 42), "  +42");
	EXPECT_EQ(cap("%+05d", 42), "+0042");
}

TEST(TinyPrintf, InteriorZeroInWidthIsNotZeroFlag)
{
	// The '0' inside "80" must not turn on zero padding.
	EXPECT_EQ(cap("%80d", 42), ref("%80d", 42));
}

TEST(TinyPrintf, WidthBeyondScratchBuffer)
{
	// Field width larger than the internal digit buffer must still work.
	EXPECT_EQ(cap("%40d", 7), std::string(39, ' ') + "7");
}

TEST(TinyPrintf, String)
{
	EXPECT_EQ(cap("%s", "abc"), "abc");
	EXPECT_EQ(cap("[%s]", ""), "[]");
}

TEST(TinyPrintf, NullString)
{
	EXPECT_EQ(cap("%s", (char *)nullptr), "(null)");
}

TEST(TinyPrintf, Char)
{
	EXPECT_EQ(cap("%c", 'A'), "A");
	EXPECT_EQ(cap("%c%c%c", 'a', 'b', 'c'), "abc");
}

TEST(TinyPrintf, Percent)
{
	EXPECT_EQ(cap("100%%"), "100%");
	EXPECT_EQ(cap("%d%%", 50), "50%");
	// %% must not swallow following characters.
	EXPECT_EQ(cap("a%%b%dc", 7), "a%b7c");
}

TEST(TinyPrintf, UnknownSpecifierEchoed)
{
	// Unknown conversion is echoed verbatim (design choice).
	EXPECT_EQ(cap("%q", 1), "%q");
}

TEST(TinyPrintf, MixedFormat)
{
	EXPECT_EQ(cap("Hello,%08x %s%c", 0x1234abcd, "world", '!'),
			  "Hello,1234abcd world!");
}

TEST(TinyPrintf, ReturnValueIsByteCount)
{
	EXPECT_EQ(ret_of("hello"), 5);
	EXPECT_EQ(ret_of("%d", -42), 3);
	EXPECT_EQ(ret_of("%5d", -42), 5);
	EXPECT_EQ(ret_of("100%%"), 4);
	EXPECT_EQ(ret_of("%s%c", "abc", '!'), 4);
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
