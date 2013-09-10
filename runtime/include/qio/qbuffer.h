#ifndef _QBUFFER_H_
#define _QBUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sys_basic.h"
#ifndef __cplusplus
#include <stdbool.h>
#endif
#include "chpl-atomics.h"

#include <inttypes.h>
#include <sys/uio.h>
#include "deque.h"

// We should have gasnett_atomic_t from sys_basic

typedef uint_least64_t qb_refcnt_base_t;
typedef atomic_uint_least64_t qbytes_refcnt_t;

#define DO_INIT_REFCNT(ptr) atomic_init_uint_least64_t (&ptr->ref_cnt, 1)
#define DO_GET_REFCNT(ptr) atomic_load_uint_least64_t (&ptr->ref_cnt)

#define DO_RETAIN(ptr) \
{ \
  /* do nothing to NULL */ \
  if( ptr ) { \
    qb_refcnt_base_t old_cnt = atomic_fetch_add_uint_least64_t (&ptr->ref_cnt, 1); \
    /* if it was 0, we couldn't have had a ref to it */ \
    if( old_cnt == 0 ) *(volatile int *)(0) = 0; /* deliberately segfault. */ \
    /* if it is now 0, we overflowed the number */ \
    if( old_cnt + 1 == 0 ) *(volatile int *)(0) = 0; /* deliberately segfault. */ \
  } \
}

#define DO_RELEASE(ptr, free_function) \
{ \
  /* do nothing to NULL */ \
  if( ptr ) { \
    qb_refcnt_base_t old_cnt = atomic_fetch_sub_uint_least64_t (&ptr->ref_cnt, 1); \
    if( old_cnt == 1 ) { \
      /* that means, after we decremented it, the count is 0. */ \
      free_function(ptr); \
    } else { \
      /* old_cnt == 0 is a fatal error (underflow) */ \
      if( old_cnt == 0 ) *(volatile int *)(0) = 0; /* deliberately segfault. */ \
    } \
  } \
}

#define DO_DESTROY_REFCNT(ptr) atomic_destroy_uint_least64_t (&ptr->ref_cnt)

// how large is an iobuf?
extern size_t qbytes_iobuf_size;

struct qbytes_s;

// a free function
typedef void (*qbytes_free_t)(struct qbytes_s*);

/* A qbytes_t is just some data with a length.
 * It is a reference-counted class type.
 * Once a qbytes_t is only the reference count may change;
 * all the other fields must remain fixed. (Note that the
 * memory pointed to by data may change).
 *
 * The free function must free both the bytes buffer as well
 * as the qbytes object itself.
 *
 * Multiple threads accessing qbytes must be managed by means
 * outside of the qbytes.
 */
typedef struct qbytes_s {
  // reference count which is atomically updated
  qbytes_refcnt_t ref_cnt;
  // all of the other fields 
  void* data;
  int64_t len;
  qbytes_free_t free_function;
  uint8_t flags; // is it const?
  uint8_t unused1;
  uint16_t unused2;
  uint32_t unused3; // this could be locale UID of the pointer!
} qbytes_t;

// These are necessary for extern class in Chapel.
typedef qbytes_t _qbytes_ptr_t;
typedef qbytes_t* qbytes_ptr_t;
#define QBYTES_PTR_NULL NULL

// increment the reference count
static inline
void qbytes_retain(qbytes_t* qb)
{
  DO_RETAIN(qb);
}

// decrement the reference count; free on 0.
static inline
void qbytes_release(qbytes_t* qb)
{
  DO_RELEASE(qb, qb->free_function);
}

// for being called by free functions... frees only the qbytes itself.
void _qbytes_free_qbytes(qbytes_t* b);
// free a NULL one
void qbytes_free_null(qbytes_t* b);
// unmap the data
void qbytes_free_munmap(qbytes_t* b);
// free the data
void qbytes_free_sys_free(qbytes_t* b);
void qbytes_free_qio_free(qbytes_t* b);

void _qbytes_init_generic(qbytes_t* ret, void* give_data, int64_t len, qbytes_free_t free_function);
err_t qbytes_create_generic(qbytes_t** out, void* give_data, int64_t len, qbytes_free_t free_function);
err_t _qbytes_init_iobuf(qbytes_t* ret);
err_t qbytes_create_iobuf(qbytes_t** out);
err_t _qbytes_init_calloc(qbytes_t* ret, int64_t len);
err_t qbytes_create_calloc(qbytes_t** out, int64_t len);

static inline
int64_t qbytes_len(qbytes_t* b)
{
  return b->len;
}

static inline
void* qbytes_data(qbytes_t* b)
{
  return b->data;
}

typedef enum {
  QB_PART_FLAGS_NONE = 0,
  QB_PART_FLAGS_EXTENDABLE_TO_ENTIRE_BYTES = 1,
} qbuffer_part_flags_t;

typedef struct qbuffer_part_s {
  // part refers to the region
  // [bytes->data + skip_bytes, bytes->data + skip_bytes + len_bytes)
  qbytes_t* bytes;
  int64_t skip_bytes;
  int64_t len_bytes; // does not include skip_bytes
  int64_t end_offset; // in bytes; subtract len_bytes from this to get start_offset
  qbuffer_part_flags_t flags;
  // for unicode strings, we might add a end_in_characters
} qbuffer_part_t;

/* A buffer is a group of bytes_t objects. It has:
 *  - some number of bytes_t objects (reference counted)
 *
 *  Buffers support O(1) push/pop on front/back of bytes, simple&fast iteration,
 *  and logarithmic indexing operations. They're not meant to support
 *  fast inserting at any position.
 *
 * Buffers are not inherently thread-safe and access should
 * be protected by some other means.
 * 
 * A buffer should be wrapped as a record; the destructor
 * should be called when it goes out of scope.
 */
typedef struct qbuffer_s {
  qbytes_refcnt_t ref_cnt; // atomically updated
  deque_t deque; // contains qbuffer_part_t s 
  int64_t offset_start;
  int64_t offset_end;
} qbuffer_t;

typedef qbuffer_t _qbuffer_ptr_t;
typedef qbuffer_t* qbuffer_ptr_t;
#define QBUFFER_PTR_NULL NULL

typedef struct qbuffer_iter_s {
  int64_t offset; // valid iter has offset_start <= offset <= offset_end
  deque_iterator_t iter;
} qbuffer_iter_t;

static inline
qbuffer_iter_t qbuffer_iter_null(void) {
  qbuffer_iter_t ret = {0, deque_iterator_null()};
  return ret;
}

void debug_print_qbuffer_iter(qbuffer_iter_t* iter);
void debug_print_qbuffer(qbuffer_t* buf);


static inline
void qbuffer_init_uninitialized(qbuffer_t* buf) {
  deque_init_uninitialized(&buf->deque);
}
static inline
int qbuffer_is_initialized(qbuffer_t* buf) {
  return deque_is_initialized(&buf->deque);
}

/* Initialize a buffer */
err_t qbuffer_init(qbuffer_t* buf);

/* Destroy a buffer inited with qbuffer_init */
err_t qbuffer_destroy(qbuffer_t* buf);

/* Destroy a buffer and free() the pointer */
err_t qbuffer_destroy_free(qbuffer_t* buf);

/* Create a reference-counted buffer ptr */
err_t qbuffer_create(qbuffer_ptr_t* out);

/* Increment a reference count
 */
static inline
void qbuffer_retain(qbuffer_ptr_t buf)
{
  DO_RETAIN(buf);
}

/* Release a reference-counted buffer ptr */
static inline
void qbuffer_release(qbuffer_ptr_t buf)
{
  DO_RELEASE(buf, qbuffer_destroy_free);
}

void qbuffer_extend_back(qbuffer_t* buf);
void qbuffer_extend_front(qbuffer_t* buf);

/* Append a bytes_t to a buffer group.
 */
err_t qbuffer_append(qbuffer_t* buf, qbytes_t* bytes, int64_t skip_bytes, int64_t len_bytes);

/* Append a buffer to another buffer; the bytes will be shared (reference counts increased) */
err_t qbuffer_append_buffer(qbuffer_t* buf, qbuffer_t* src, qbuffer_iter_t src_start, qbuffer_iter_t src_end);

/* Prepend a bytes_t to a buffer group.
 */
err_t qbuffer_prepend(qbuffer_t* buf, qbytes_t* bytes, int64_t skip_bytes, int64_t len_bytes);

/* trim functions remove parts that are completely in the area
 * to be removed. */
void qbuffer_trim_front(qbuffer_t* buf, int64_t remove_bytes);
void qbuffer_trim_back(qbuffer_t* buf, int64_t remove_bytes);

/* Remove a part from the front or back. */
err_t qbuffer_pop_front(qbuffer_t* buf);
err_t qbuffer_pop_back(qbuffer_t* buf);

/* Without changing any memory, changes the offsets
 * used in the buffer (offset_start, offset_end, and offsets in the parts).
 */
void qbuffer_reposition(qbuffer_t* buf, int64_t new_offset_start);

qbuffer_iter_t qbuffer_begin(qbuffer_t* buf);
qbuffer_iter_t qbuffer_end(qbuffer_t* buf);

static inline
ssize_t qbuffer_num_parts(qbuffer_t* buf)
{
  return deque_size(sizeof(qbuffer_part_t), & buf->deque);
}

/* do a and b refer to the same part?
 */
static inline
char qbuffer_iter_same_part(qbuffer_iter_t a, qbuffer_iter_t b) {
  return deque_it_equals(a.iter, b.iter);
}

/* Moves to the beginning of the next part
 */
void qbuffer_iter_next_part(qbuffer_t* buf, qbuffer_iter_t* iter);

/* Moves to the beginning of the previous part
 */
void qbuffer_iter_prev_part(qbuffer_t* buf, qbuffer_iter_t* iter);

/* Move to the beginning of the current part 
 */
void qbuffer_iter_floor_part(qbuffer_t* buf, qbuffer_iter_t* iter);
/* Move to the end of the current part.
 */
void qbuffer_iter_ceil_part(qbuffer_t* buf, qbuffer_iter_t* iter);

/* Advances an iterator using linear search. 
 * Returns qbuffer_start() if the resulting offset was < start
 *         qbuffer_end() if the resulting offset was >= end
 */
void qbuffer_iter_advance(qbuffer_t* buf, qbuffer_iter_t* iter, int64_t amt);

/* Find offset in window in logarithmic time.
 * Note these offsets start at buf->offset_start, not 0.
 *
 * Returns qbuffer_start() if the offset was < start
 *         qbuffer_end() if the offset was >= end
 */
qbuffer_iter_t qbuffer_iter_at(qbuffer_t* buf, int64_t offset);

/* How many parts are in an iterator? end >= start
 * always returns 1 + end - start
 */
static inline
ssize_t qbuffer_iter_num_parts(qbuffer_iter_t start, qbuffer_iter_t end)
{
  return 1 + deque_it_difference(sizeof(qbuffer_part_t), end.iter, start.iter);
}

static inline
char qbuffer_iter_equals(qbuffer_iter_t start, qbuffer_iter_t end)
{
  return start.offset == end.offset;
}

/* How many bytes are in an iterator? end >= start
 */
static inline
int64_t qbuffer_iter_num_bytes(qbuffer_iter_t start, qbuffer_iter_t end)
{
  return end.offset - start.offset;
}

/* How many bytes are after an iterator?
 */
static inline
int64_t qbuffer_iter_num_bytes_after(qbuffer_t* buf, qbuffer_iter_t iter)
{
  return buf->offset_end - iter.offset;
}

/* How many bytes are before an iterator?
 */
static inline
int64_t qbuffer_iter_num_bytes_before(qbuffer_t* buf, qbuffer_iter_t iter)
{
  return iter.offset - buf->offset_start;
}

// Returns whats in the iterator. If the caller wants to hold on to
// the bytes, it should retain them (this function does not).
static inline
void qbuffer_iter_get(qbuffer_iter_t iter, qbuffer_iter_t end, qbytes_t** bytes_out, int64_t* skip_out, int64_t* len_out)
{
  qbuffer_part_t* qbp = (qbuffer_part_t*) deque_it_get_cur_ptr(sizeof(qbuffer_part_t), iter.iter);
  int64_t iter_offset_within = iter.offset - (qbp->end_offset - qbp->len_bytes);
  int64_t part_len = qbp->len_bytes - iter_offset_within;
  int64_t len = end.offset - iter.offset;
  
  if( len > part_len ) len = part_len;

  *bytes_out = qbp->bytes;
  *skip_out = qbp->skip_bytes + iter_offset_within;
  *len_out = len;
}

static inline
int64_t qbuffer_start_offset(qbuffer_t* buf)
{
  return buf->offset_start;
}

static inline
int64_t qbuffer_end_offset(qbuffer_t* buf)
{
  return buf->offset_end;
}

static inline
int64_t qbuffer_len(qbuffer_t* buf)
{
  return buf->offset_end - buf->offset_start;
}

/* Turn a range from a qbuffer into an io-vector. Note that this contains
 * pointers into the qbuffer and is only valid until the qbuffer is changed...
 */
err_t qbuffer_to_iov(qbuffer_t* buf, qbuffer_iter_t start, qbuffer_iter_t end, 
                   size_t max_iov, struct iovec *iov_out, 
                   qbytes_t** bytes_out /* can be NULL */,
                   size_t *iovcnt_out);

/* Returns a single bytes object for the qbuffer.
 * This is like the C++ string.c_str() method;
 *
 * It is the responsibility of the caller to call
 * qbytes_release on the result of this function.
 */
err_t qbuffer_flatten(qbuffer_t* buf, qbuffer_iter_t start, qbuffer_iter_t end, qbytes_t** bytes_out);

/* Create a reference-sharing version of the buffer,
 * starting from current iterator position.
 */
//err_t qbuffer_clone(qbuffer_t* buf, qbuffer_iter_t start, qbuffer_iter_t end, qbuffer_ptr_t* buf_out);

/* Copies bytes from start to end in buffer to ptr.
 * Returns an error if we would exceed ret_len
 * */
err_t qbuffer_copyout(qbuffer_t* buf, qbuffer_iter_t start, qbuffer_iter_t end, void* ptr, size_t ret_len);

/* Copies bytes from ptr to start to end in buffer.
 * Returns an error if we would exceed ret_len
 * */
err_t qbuffer_copyin(qbuffer_t* buf, qbuffer_iter_t start, qbuffer_iter_t end, const void* ptr, size_t ret_len);

err_t qbuffer_copyin_buffer(qbuffer_t* dst, qbuffer_iter_t dst_start, qbuffer_iter_t dst_end,
                            qbuffer_t* src, qbuffer_iter_t src_start, qbuffer_iter_t src_end);

/* Overwrites the qbuffer buffers with a fixed byte.
 * */
err_t qbuffer_memset(qbuffer_t* buf, qbuffer_iter_t start, qbuffer_iter_t end, unsigned char byte);

// How many bytes to try to store on stack in some functions that don't
// really want to call malloc
#define MAX_ON_STACK 128

#ifdef _chplrt_H_

#include "chpl-mem.h"
#define qio_malloc(size) chpl_mem_alloc(size, CHPL_RT_MD_IO_BUFFER, __LINE__, __FILE__)
#define qio_calloc(nmemb, size) chpl_mem_allocManyZero(nmemb, size, CHPL_RT_MD_IO_BUFFER, __LINE__, __FILE__)
#define qio_realloc(ptr, size) chpl_mem_realloc(ptr, size, CHPL_RT_MD_IO_BUFFER, __LINE__, __FILE__)
#define qio_free(ptr) chpl_mem_free(ptr, __LINE__, __FILE__)

static inline char* qio_strdup(const char* ptr)
{
  char* ret = (char*) qio_malloc(strlen(ptr)+1);
  if( ret ) strcpy(ret, ptr);
  return ret;
}

typedef chpl_bool qio_bool;

#else

#define qio_malloc(size) malloc(size)
#define qio_calloc(nmemb, size) calloc(nmemb,size)
#define qio_realloc(ptr, size) realloc(ptr, size)
#define qio_free(ptr) free(ptr)
#define sys_free(ptr) free(ptr)
#define qio_strdup(ptr) strdup(ptr)

typedef bool qio_bool;

#endif

// Declare MAX_ON_STACK bytes. We declare it as uint64_t to
// make sure it's aligned as well as malloc would be.
#define MAYBE_STACK_SPACE(type,onstack) \
  type onstack[MAX_ON_STACK/sizeof(type)]

#define MAYBE_STACK_ALLOC(type, count, ptr, onstack) \
{ \
  size_t size = count * sizeof(type); \
  if( size <= sizeof(onstack) ) { \
    ptr = onstack; \
  } else { \
    ptr = (type*) qio_malloc(size); \
  } \
}


#define MAYBE_STACK_FREE(ptr, onstack) \
{ \
  if (ptr && ptr != onstack) { \
    qio_free(ptr); \
  } \
}

#define VOID_PTR_DIFF(a,b) (((intptr_t) (a)) - ((intptr_t) (b)))
#define VOID_PTR_ADD(ptr,amt) ((void*)(((char*) (ptr)) + (amt)))
#define VOID_PTR_ALIGN(ptr,align) (((uintptr_t)ptr) & (align - 1))

#ifdef __cplusplus
} // end extern "C"
#endif


#endif

