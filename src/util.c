#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "numtypes.h"
#include "util.h"

/**
 * calls `free` on an object.
 * This function is made to adapt stdlib.h/free to the function
 * signature for node#.p_unref_from.
 */
void* free_from(void* this, void* start) {
	if (this != start)
		free(this);
	return NULL;
}

static bool numstr_can_be_printed(u8* this, fast_u8 len) {
	if (!len)
		return true;
	if ((*this&224) == 192)
		return true;
	if (*this >= 1 && *this < 32)
		return false;
	if (*this >= 128)
		return false;
	return numstr_can_be_printed(&this[1], len - 1);
}
static bool numstr_can_be_printed_like_identifier(u8* this, fast_u8 len) {
	fast_u8 i;
	if (*this < 65)
		return false;
	for (i = 0; i < len; i++) {
		if (!this[i] && i)
			break;
		if (
			this[i] <  48 ||
			this[i] >= 91 && this[i] <  96 ||
			this[i] >= 123
		) return false;
	}
	for (; i < len; i++)
		if (this[i])
			return false;
	return true;
}
bool u32_can_be_printed(u32 this) {
	if (!this)
		return true;
	return numstr_can_be_printed((u8*)&this, 4);
}
bool u32_can_be_printed_like_identifier(u32 this) {
	return numstr_can_be_printed_like_identifier((u8*)&this, 4);
}
bool u64_can_be_printed(u64 this) {
	return !this ?
		true :
		numstr_can_be_printed((u8*)&this, 8);
}
bool u64_can_be_printed_like_identifier(u64 this) {
	return numstr_can_be_printed_like_identifier((u8*)&this, 8);
}
void print_rimek_byte(u8 x) {
	char* small = "ₒ☺☻♥♦♣♠•◘○◙♂♀♪♫☼►◄↕‼¶\0§\0▬↨↑↓→←∟↔▲▼";
	char* mediu = "ÇüéâäàåçêëèïîìÄÅÉæÆôöòûùÿÖÜ¢£¥..ƒáíóúñÑªº¿..¬½¼¡«»";
	char* large =
		"░▒▓│┤╡╢╖╕╣║╗╝╜╛┐└┴┬├─┼╞╟╚╔╩╦╠═╬╧╨╤╥╙╘╒╓╫╪┘┌█▄▌▐▀"
		"α\0ß\0Γ\0π\0Σ\0σ\0µ\0τ\0Φ\0Θ\0Ω\0δ\0∞φ\0ε\0∩"
		"≡±\0≥≤⌠⌡÷\0≈°\0∙·\0√ⁿ²\0■ȳ\0";
	if (x < 32)
		printf("%.3s", &small[3*x]);
	else if (x >= 32 && x < 127)
		putchar(x);
	else if (x == 127)
		printf("⌂");
	else if (x == 158)
		printf("₧");
	else if (x == 169)
		printf("⌐");
	else if (x >= 128 && x < 176)
		printf("%.2s", &mediu[2*(x - 128)]);
	else if (x >= 176)
		printf("%.3s", &large[3*(x - 176)]);
	else printf("oops (%u)\n", x);
}
void print_rimek_bytes(void* x, unt len) {
	if (!len) return;
	print_rimek_byte(*(u8*)x);
	print_rimek_bytes(x + 1, len - 1);
}
