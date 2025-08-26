// WorkbenchOS
// Copyright (c) 2021 Matthew Costa <ucosty@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-only
#include <LibStd/Convert.h>
#include <LibStd/Types.h>
#include <UnbufferedConsole.h>

struct FormatSpec {
    char type;      // 'd','u','x','X','o','b','p','s','c' (we'll use a subset)
    char align;     // '<','>','^'
    char fill;      // fill character, default ' '
    char sign;      // 0 (none), '+', ' '
    bool alt;       // '#'
    bool zero;      // '0' (only meaningful for right-align)
    int  width;     // -1 means not specified
    bool uppercase; // for hex/bin prefixes and digits
};

static void emit_n(const char ch, int count) {
    while (count-- > 0) debug_putchar(ch);
}

static void emit_str(const char* s, const int n) {
    for (int i = 0; i < n; ++i) debug_putchar(s[i]);
}

static bool is_digit(const char c) { return c >= '0' && c <= '9'; }

void print_literal_until_field(Formatter* f) {
    // Prints chars until a single '{' that begins a field, supporting escapes:
    // "{{" -> "{", "}}" -> "}"
    const char* fmt = f->fmt;
    while (true) {
        const char c = fmt[f->index];
        if (c == '\0') return;
        if (c == '{') {
            char n = fmt[f->index + 1];
            if (n == '{') { // escaped
                debug_putchar('{');
                f->index += 2;
                continue;
            }
            // Real field start
            return;
        }
        if (c == '}') {
            if (const char n = fmt[f->index + 1]; n == '}') { // escaped
                debug_putchar('}');
                f->index += 2;
                continue;
            }
            // A lone '}' (undefined) - just print it.
        }
        debug_putchar(c);
        ++f->index;
    }
}

// Parse a spec at f->fmt[f->index], assuming current char is '{'.
// Consumes "{...}" and leaves f->index pointing past the closing '}'.
// On "{}" sets defaults. On "{:...}" parses subset described above.
static void parse_spec(Formatter* f, FormatSpec* out) {
    // Defaults
    out->type = 0;
    out->align = '>';      // right
    out->fill = ' ';
    out->sign = 0;
    out->alt = false;
    out->zero = false;
    out->width = -1;
    out->uppercase = false;

    const char* s = f->fmt;
    size_t i = f->index;

    // Expect '{'
    ++i;

    // "{}" case
    if (s[i] == '}') {
        f->index = i + 1;
        return;
    }

    // Optional colon
    if (s[i] == ':') {
        ++i;

        // Optional (fill)(align) or just (align)
        // If s[i+1] is an align char, treat s[i] as fill.
        if (s[i] && (s[i+1] == '<' || s[i+1] == '>' || s[i+1] == '^')) {
            out->fill = s[i];
            out->align = s[i+1];
            i += 2;
        } else if (s[i] == '<' || s[i] == '>' || s[i] == '^') {
            out->align = s[i];
            ++i;
        }

        // sign
        if (s[i] == '+' || s[i] == ' ') { out->sign = s[i]; ++i; }

        // alternate form
        if (s[i] == '#') { out->alt = true; ++i; }

        // zero pad flag
        if (s[i] == '0') { out->zero = true; out->fill = '0'; ++i; }

        // width
        if (is_digit(s[i])) {
            int w = 0;
            while (is_digit(s[i])) { w = w * 10 + (s[i] - '0'); ++i; }
            out->width = w;
        }

        // type
        if (s[i] && s[i] != '}') {
            out->type = s[i];
            if (out->type == 'X') out->uppercase = true;
            ++i;
        }
    }

    // Expect closing '}'
    if (s[i] == '}') {
        f->index = i + 1;
    } else {
        // Recover by scanning to next '}'
        while (s[i] && s[i] != '}') ++i;
        if (s[i] == '}') ++i;
        f->index = i;
    }
}

static int format_uint_rev(u64 v, unsigned base, bool uppercase, char* end) {
    const auto digs_l = "0123456789abcdef";
    const auto digs_u = "0123456789ABCDEF";
    const char* D = uppercase ? digs_u : digs_l;

    // Write backward, return length
    char* p = end;
    do {
        const auto d = static_cast<unsigned>(v % base);
        *--p = D[d];
        v /= base;
    } while (v);
    return static_cast<int>(end - p); // number of digits
}

static void emit_padded_numeric(const char* sign,
                                const char* prefix, int prefix_len,
                                const char* digits, int ndigits,
                                const FormatSpec& fs) {
    int sign_len = (sign && *sign) ? 1 : 0;
    int core_len = sign_len + prefix_len + ndigits;

    int pad = 0;
    if (fs.width > core_len) pad = fs.width - core_len;

    // Right align with zero pad: zeros go after sign+prefix
    if (fs.align == '>' && fs.zero) {
        if (sign_len) debug_putchar(*sign);
        if (prefix_len) emit_str(prefix, prefix_len);
        emit_n('0', pad);
        emit_str(digits, ndigits);
        return;
    }

    // Other alignment
    int left_pad = 0, right_pad = 0;
    if (fs.align == '<') {
        right_pad = pad;
    } else if (fs.align == '^') {
        left_pad = pad / 2;
        right_pad = pad - left_pad;
    } else { // '>'
        left_pad = pad;
    }

    emit_n(fs.fill, left_pad);
    if (sign_len) debug_putchar(*sign);
    if (prefix_len) emit_str(prefix, prefix_len);
    emit_str(digits, ndigits);
    emit_n(fs.fill, right_pad);
}

template <typename Unsigned>
static void format_and_emit_unsigned(Unsigned uv, const FormatSpec& fs, bool is_pointer = false) {
    // Determine base/type
    char t = fs.type ? fs.type : 'u';
    unsigned base = 10;
    bool upper = fs.uppercase;

    if (t == 'c') {
        emit_n(uv, 1);
        return;
    }

    if (t == 'x' || t == 'X' || (is_pointer && t == 0)) { base = 16; }
    else if (t == 'o') { base = 8; }
    else if (t == 'b') { base = 2; }
    else if (t == 'p') { base = 16; }
    else { base = 10; }

    // Convert
    char buf[2 + 64 + 1]; // room for prefix + 64 bin digits + NUL (NUL not needed)
    char* end = buf + sizeof(buf);
    int ndigits = format_uint_rev((u64)uv, base, upper, end);
    const char* digits = end - ndigits;

    // Prefixes
    char pref[2];
    int pref_len = 0;

    if (is_pointer || t == 'p') {
        pref[0] = '0'; pref[1] = upper ? 'X' : 'x'; pref_len = 2;
    } else if (fs.alt) {
        if (base == 16) { pref[0] = '0'; pref[1] = upper ? 'X' : 'x'; pref_len = 2; }
        else if (base == 8) { pref[0] = '0'; pref_len = 1; }        // C-style alt for octal
        else if (base == 2) { pref[0] = '0'; pref[1] = upper ? 'B' : 'b'; pref_len = 2; }
    }

    emit_padded_numeric(nullptr, pref, pref_len, digits, ndigits, fs);
}

template <typename Signed>
static void format_and_emit_signed(Signed sv, const FormatSpec& fs) {
    // Choose base/type
    char t = fs.type ? fs.type : 'd';
    unsigned base = 10;
    bool upper = fs.uppercase;

    if (t == 'c') {
        emit_n(sv, 1);
        return;
    }

    if (t == 'x' || t == 'X') base = 16;
    else if (t == 'o') base = 8;
    else if (t == 'b') base = 2;
    else base = 10;

    // Convert magnitude
    bool neg = sv < 0 && base == 10; // only show '-' for decimal
    u64 uv;
    if (sv < 0) {
        // Avoid overflow on INT_MIN
        uv = (u64)(-(sv + (Signed)1)) + 1u;
    } else {
        uv = (u64)sv;
    }

    char num[2 + 64];
    char* end = num + sizeof(num);
    int ndigits = format_uint_rev(uv, base, upper, end);
    const char* digits = end - ndigits;

    // Prefixes & sign
    char pref[2];
    int pref_len = 0;
    if (fs.alt) {
        if (base == 16) { pref[0]='0'; pref[1]= upper ? 'X':'x'; pref_len=2; }
        else if (base == 8) { pref[0]='0'; pref_len=1; }
        else if (base == 2) { pref[0]='0'; pref[1]= upper ? 'B':'b'; pref_len=2; }
    }

    char sign = 0;
    if (base == 10) {
        if (neg) sign = '-';
        else if (fs.sign == '+') sign = '+';
        else if (fs.sign == ' ') sign = ' ';
    }

    // Emit with padding. Zero-padding applies after sign/prefix.
    // Use the same helper but pass sign separately.
    int sign_len = sign ? 1 : 0;
    int core_len = sign_len + pref_len + ndigits;
    int pad = 0;
    if (fs.width > core_len) pad = fs.width - core_len;

    if (fs.align == '>' && fs.zero) {
        if (sign) debug_putchar(sign);
        if (pref_len) emit_str(pref, pref_len);
        emit_n('0', pad);
        emit_str(digits, ndigits);
        return;
    }

    int left_pad = 0, right_pad = 0;
    if (fs.align == '<') right_pad = pad;
    else if (fs.align == '^') { left_pad = pad / 2; right_pad = pad - left_pad; }
    else left_pad = pad;

    emit_n(fs.fill, left_pad);
    if (sign) debug_putchar(sign);
    if (pref_len) emit_str(pref, pref_len);
    emit_str(digits, ndigits);
    emit_n(fs.fill, right_pad);
}

void println_internal(Formatter* f) {
    const char* s = f->fmt;
    while (s[f->index] != '\0') {
        char c = s[f->index];
        if (c == '{') {
            if (s[f->index + 1] == '{') { debug_putchar('{'); f->index += 2; }
            else { debug_putchar('{'); ++f->index; } // unmatched field => literal
        } else if (c == '}') {
            if (s[f->index + 1] == '}') { debug_putchar('}'); f->index += 2; }
            else { debug_putchar('}'); ++f->index; }
        } else {
            debug_putchar(c);
            ++f->index;
        }
    }
}

void select(Formatter* f, Std::String *s) {
    FormatSpec fs{};
    parse_spec(f, &fs);

    // Strings: we only honor width/align/fill here
    const int n = s->length();
    int pad = (fs.width > n) ? (fs.width - n) : 0;
    int left = 0, right = 0;
    if (fs.align == '<') right = pad;
    else if (fs.align == '^') { left = pad/2; right = pad - left; }
    else left = pad;
    emit_n(fs.fill, left);
    for (int i = 0; i < n; ++i) debug_putchar(s->get(i));
    emit_n(fs.fill, right);
}

void select(Formatter* f, const Std::String& s) {
    select(f, const_cast<Std::String *>(&s));
}

void select(Formatter* f, Std::StringView sv) {
    FormatSpec fs{};
    parse_spec(f, &fs);

    const int n = sv.length();
    int pad = (fs.width > n) ? (fs.width - n) : 0;
    int left = 0, right = 0;
    if (fs.align == '<') right = pad;
    else if (fs.align == '^') { left = pad/2; right = pad - left; }
    else left = pad;
    emit_n(fs.fill, left);
    for (int i = 0; i < n; ++i) debug_putchar(sv.get(i));
    emit_n(fs.fill, right);
}

void select(Formatter* f, const char* cstr) {
    FormatSpec fs{};
    parse_spec(f, &fs);

    int n = 0; while (cstr[n] != '\0') ++n;
    int pad = (fs.width > n) ? (fs.width - n) : 0;
    int left = 0, right = 0;
    if (fs.align == '<') right = pad;
    else if (fs.align == '^') { left = pad/2; right = pad - left; }
    else left = pad;
    emit_n(fs.fill, left);
    emit_str(cstr, n);
    emit_n(fs.fill, right);
}


void select(Formatter* f, const u8 v) {
    FormatSpec fs{};
    parse_spec(f, &fs);

    format_and_emit_unsigned<u8>(v, fs, false);
}


void select(Formatter* f, const u32 v) {
    FormatSpec fs{};
    parse_spec(f, &fs);

    format_and_emit_unsigned<u32>(v, fs, false);
}

void select(Formatter* f, const u64 v) {
    FormatSpec fs{};
    parse_spec(f, &fs);

    format_and_emit_unsigned<u64>(v, fs, false);
}

void select(Formatter* f, const int32_t v) {
    FormatSpec fs{};
    parse_spec(f, &fs);

    format_and_emit_signed<int32_t>(v, fs);
}

void select(Formatter* f, int64_t v) {
    FormatSpec fs{};
    parse_spec(f, &fs);

    format_and_emit_signed<int64_t>(v, fs);
}

void select(Formatter* f, void* p) {
    FormatSpec fs{};
    parse_spec(f, &fs);

    // Force 'p' behavior unless user explicitly chose a type
    if (!fs.type) fs.type = 'p';
    format_and_emit_unsigned<u64>(reinterpret_cast<uintptr_t>(p), fs, true);
}

void select(Formatter* f, const bool b) {
    FormatSpec fs{};
    parse_spec(f, &fs);

    const char* s = b ? "true" : "false";
    select(f, s);
}

void select(Formatter* f, const char c) {
    FormatSpec fs{};
    parse_spec(f, &fs);

    const char tmp[1] = { c };
    const int pad = (fs.width > 1) ? (fs.width - 1) : 0;
    int left = 0, right = 0;
    if (fs.align == '<') right = pad;
    else if (fs.align == '^') { left = pad/2; right = pad - left; }
    else left = pad;
    emit_n(fs.fill, left);
    emit_str(tmp, 1);
    emit_n(fs.fill, right);
}
