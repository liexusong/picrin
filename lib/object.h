/**
 * See Copyright Notice in picrin.h
 */

#ifndef PICRIN_OBJECT_H
#define PICRIN_OBJECT_H

#if defined(__cplusplus)
extern "C" {
#endif

#include "khash.h"

#define OBJECT_HEADER                           \
  struct object *next;                          \
  unsigned char tt;

#define TYPE_MASK 0x7f
#define GC_MARK 0x80

struct object {
  OBJECT_HEADER
};

struct blob {
  OBJECT_HEADER
  unsigned char *data;
  int len;
};

#define ROPE_HEADER                             \
  OBJECT_HEADER                                 \
  int len;

struct rope {
  ROPE_HEADER
};

struct rope_leaf {
  ROPE_HEADER
  const char *str;
};

struct rope_node {
  ROPE_HEADER
  struct rope *s1;
  struct rope *s2;
};

struct string {
  OBJECT_HEADER
  struct rope *rope;
};

struct symbol {
  OBJECT_HEADER
  struct string *str;
};

struct pair {
  OBJECT_HEADER
  pic_value car;
  pic_value cdr;
};

struct vector {
  OBJECT_HEADER
  pic_value *data;
  int len;
};

KHASH_DECLARE(dict, struct symbol *, pic_value)

struct dict {
  OBJECT_HEADER
  khash_t(dict) hash;
};

KHASH_DECLARE(attr, struct object *, pic_value)

struct attr {
  OBJECT_HEADER
  khash_t(attr) hash;
  struct attr *prev;         /* for GC */
};

struct data {
  OBJECT_HEADER
  const pic_data_type *type;
  void *data;
};

struct record {
  OBJECT_HEADER
  struct symbol *type;
  pic_value datum;
};

enum {
  OP_HALT  = 0x00,        /* 0x00                 OP_HALT           */
  OP_CALL  = 0x01,        /* 0x01 0x**            OP_CALL argc      */
  OP_PROC  = 0x02,        /* 0x02 0x** 0x**       OP_PROC dest irep */
  OP_LOAD  = 0x03,        /* 0x03 0x** 0x**       OP_LOAD dest i    */
  OP_LREF  = 0x04,        /* 0x04 0x** 0x** 0x**  OP_LREF dest n i  */
  OP_LSET  = 0x05,        /* 0x05 0x** 0x** 0x**  OP_LSET src n i   */
  OP_GREF  = 0x06,        /* 0x06 0x** 0x**       OP_GREF dest i    */
  OP_GSET  = 0x07,        /* 0x07 0x** 0x**       OP_GSET src i     */
  OP_COND  = 0x08,        /* 0x08 0x** 0x** 0x**  OP_COND c offset  */
  OP_LOADT = 0x09,        /* 0x09 0x**            OP_LOADT dest     */
  OP_LOADF = 0x0A,        /* 0x0A 0x**            OP_LOADF dest     */
  OP_LOADN = 0x0B,        /* 0x0B 0x**            OP_LOADN dest     */
  OP_LOADU = 0x0C,        /* 0x0C 0x**            OP_LOADU dest     */
  OP_LOADI = 0x0D         /* 0x0D 0x** 0x**       OP_LOADI dest i   */
};

typedef unsigned char code_t;

#define IREP_VARG 1
#define IREP_CODE_STATIC 2

struct irep {
  OBJECT_HEADER
  unsigned char argc;
  unsigned char flags;
  unsigned char frame_size;
  unsigned char irepc, objc;
  size_t codec;
  struct irep **irep;
  pic_value *obj;
  const code_t *code;
};

struct frame {
  OBJECT_HEADER
  unsigned char regc;
  pic_value *regs;
  struct frame *up;
};

struct proc {
  OBJECT_HEADER
  union {
    pic_func_t func;
    struct irep *irep;
  } u;
  struct frame *env;
};

#define TYPENAME_int   "integer"
#define TYPENAME_blob  "bytevector"
#define TYPENAME_char  "character"
#define TYPENAME_sym   "symbol"
#define TYPENAME_proc  "procedure"
#define TYPENAME_str   "string"
#define TYPENAME_vec   "vector"

#define TYPE_CHECK(pic, v, type) do {                           \
    if (! pic_##type##_p(pic, v))                               \
      pic_error(pic, TYPENAME_##type " required", 1, v);        \
  } while (0)

#define VALID_INDEX(pic, len, i) do {                                   \
    if (i < 0 || len <= i) pic_error(pic, "index out of range", 1, pic_int_value(pic, i)); \
  } while (0)
#define VALID_RANGE(pic, len, s, e) do {                                \
    if (s < 0 || len < s) pic_error(pic, "invalid start index", 1, pic_int_value(pic, s)); \
    if (e < s || len < e) pic_error(pic, "invalid end index", 1, pic_int_value(pic, e)); \
  } while (0)
#define VALID_ATRANGE(pic, tolen, at, fromlen, s, e) do {               \
    VALID_INDEX(pic, tolen, at);                                        \
    VALID_RANGE(pic, fromlen, s, e);                                    \
    if (tolen - at < e - s) pic_error(pic, "invalid range", 0);        \
  } while (0)

PIC_STATIC_INLINE int
obj_type(void *ptr)
{
  return ((struct object *) ptr)->tt & TYPE_MASK;
}

PIC_STATIC_INLINE pic_value
obj_value(pic_state *pic, void *ptr)
{
  return pic_obj_value(pic, ptr, obj_type(ptr));
}

#define DEFPTR(name,type)                       \
  PIC_STATIC_INLINE type *                      \
  name##_ptr(pic_state *pic, pic_value o) {     \
    assert(pic_##name##_p(pic,o));              \
    return (type *) pic_ptr(pic, o);            \
  }

#define pic_data_p(pic,o) (pic_data_p(pic,o,NULL))
DEFPTR(sym, struct symbol)
DEFPTR(str, struct string)
DEFPTR(blob, struct blob)
DEFPTR(pair, struct pair)
DEFPTR(vec, struct vector)
DEFPTR(dict, struct dict)
DEFPTR(attr, struct attr)
DEFPTR(data, struct data)
DEFPTR(proc, struct proc)
DEFPTR(rec, struct record)
DEFPTR(irep, struct irep)
#undef pic_data_p

struct object *pic_obj_alloc(pic_state *, int type);
struct object *pic_obj_alloc_unsafe(pic_state *, int type);

struct frame *pic_make_frame_unsafe(pic_state *, int n);
pic_value pic_make_proc_irep_unsafe(pic_state *, struct irep *, struct frame *);
pic_value pic_make_record(pic_state *, pic_value type, pic_value datum);
pic_value pic_record_type(pic_state *pic, pic_value record);
pic_value pic_record_datum(pic_state *pic, pic_value record);
pic_value pic_make_cont(pic_state *pic, pic_value k);
int pic_str_hash(pic_state *pic, pic_value str);
int pic_str_cmp(pic_state *pic, pic_value str1, pic_value str2);

void pic_warnf(pic_state *pic, const char *fmt, ...); /* deprecated */

#if defined(__cplusplus)
}
#endif

#endif
