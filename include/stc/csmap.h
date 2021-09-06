/* MIT License
 *
 * Copyright (c) 2021 Tyge Løvset, NORCE, www.norceresearch.no
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef CSMAP_H_INCLUDED
#define CSMAP_H_INCLUDED

// Sorted/Ordered set and map - implemented as an AA-tree.
/*
#include <stdio.h>
#include <stc/csmap.h>
using_csmap(mx, int, char); // Sorted map<int, char>

int main(void) {
    c_forvar (csmap_mx m = csmap_mx_init(), csmap_mx_del(&m))
    {
        csmap_mx_insert(&m, 5, 'a');
        csmap_mx_insert(&m, 8, 'b');
        csmap_mx_insert(&m, 12, 'c');

        csmap_mx_iter_t it = csmap_mx_find(&m, 10); // none
        char val = csmap_mx_find(&m, 5).ref->second;
        csmap_mx_put(&m, 5, 'd'); // update
        csmap_mx_erase(&m, 8);

        c_foreach (i, csmap_mx, m)
            printf("map %d: %c\n", i.ref->first, i.ref->second);
    }
}
*/
#include "ccommon.h"
#include "template.h"
#include <stdlib.h>
#include <string.h>

#define KEY_REF_csmap_(vp)   (&(vp)->first)
struct csmap_rep { size_t root, disp, head, size, cap; void* nodes[]; };
#define _csmap_rep(self) c_container_of((self)->nodes, struct csmap_rep, nodes)

#if !defined cx_forwarded
    _c_aatree_types(Self, C, i_KEY, i_VAL);
#endif

\
    cx_MAP_ONLY( struct cx_value_t { \
        cx_key_t first; \
        cx_mapped_t second; \
    }; ) \
\
    struct cx_node_t { \
        cx_memb(_size_t) link[2]; \
        int8_t level; \
        cx_value_t value; \
    }; \
\
    typedef i_KEYRAW cx_rawkey_t; \
    typedef i_VALRAW cx_memb(_rawmapped_t); \
    typedef cx_SET_ONLY( i_KEYRAW ) \
            cx_MAP_ONLY( struct { i_KEYRAW first; \
                                   i_VALRAW second; } ) \
    cx_rawvalue_t; \
\
    STC_API Self               cx_memb(_init)(void); \
    STC_API Self               cx_memb(_clone)(Self tree); \
    STC_API void             cx_memb(_del)(Self* self); \
    STC_API void             cx_memb(_reserve)(Self* self, size_t cap); \
    STC_API cx_value_t*    cx_memb(_find_it)(const Self* self, i_KEYRAW rkey, cx_iter_t* out); \
    STC_API cx_iter_t      cx_memb(_lower_bound)(const Self* self, i_KEYRAW rkey); \
    STC_API cx_value_t*    cx_memb(_front)(const Self* self); \
    STC_API cx_value_t*    cx_memb(_back)(const Self* self); \
    STC_API int              cx_memb(_erase)(Self* self, i_KEYRAW rkey); \
    STC_API cx_iter_t      cx_memb(_erase_at)(Self* self, cx_iter_t it); \
    STC_API cx_iter_t      cx_memb(_erase_range)(Self* self, cx_iter_t it1, cx_iter_t it2); \
    STC_API cx_result_t    cx_memb(_insert_entry_)(Self* self, i_KEYRAW rkey); \
    STC_API void             cx_memb(_next)(cx_iter_t* it); \
\
    STC_INLINE bool          cx_memb(_empty)(Self tree) { return _csmap_rep(&tree)->size == 0; } \
    STC_INLINE size_t        cx_memb(_size)(Self tree) { return _csmap_rep(&tree)->size; } \
    STC_INLINE size_t        cx_memb(_capacity)(Self tree) { return _csmap_rep(&tree)->cap; } \
    STC_INLINE void          cx_memb(_clear)(Self* self) { cx_memb(_del)(self); *self = cx_memb(_init)(); } \
    STC_INLINE void          cx_memb(_swap)(Self* a, Self* b) {c_swap(Self, *a, *b); } \
    STC_INLINE bool          cx_memb(_contains)(const Self* self, i_KEYRAW rkey) \
                                {cx_iter_t it; return cx_memb(_find_it)(self, rkey, &it) != NULL; } \
    STC_INLINE cx_value_t* cx_memb(_get)(const Self* self, i_KEYRAW rkey) \
                                {cx_iter_t it; return cx_memb(_find_it)(self, rkey, &it); } \
\
    STC_INLINE Self \
    cx_memb(_with_capacity)(size_t size) { \
        Self tree = cx_memb(_init)(); \
        cx_memb(_reserve)(&tree, size); \
        return tree; \
    } \
\
    STC_INLINE cx_rawvalue_t \
    cx_memb(_value_toraw)(cx_value_t* val) { \
        return cx_SET_ONLY( i_KEYTO(val) ) \
               cx_MAP_ONLY( c_make(cx_rawvalue_t){i_KEYTO(&val->first), i_VALTO(&val->second)} ); \
    } \
    STC_INLINE void \
    cx_memb(_value_del)(cx_value_t* val) { \
        i_KEYDEL(cx_keyref(val)); \
        cx_MAP_ONLY( i_VALDEL(&val->second); ) \
    } \
    STC_INLINE void \
    cx_memb(_value_clone)(cx_value_t* dst, cx_value_t* val) { \
        *cx_keyref(dst) = i_KEYFROM(i_KEYTO(cx_keyref(val))); \
        cx_MAP_ONLY( dst->second = i_VALFROM(i_VALTO(&val->second)); ) \
    } \
\
    STC_INLINE cx_iter_t \
    cx_memb(_find)(const Self* self, i_KEYRAW rkey) { \
        cx_iter_t it; \
        cx_memb(_find_it)(self, rkey, &it); \
        return it; \
    } \
\
    STC_INLINE cx_result_t \
    cx_memb(_emplace)(Self* self, i_KEYRAW rkey cx_MAP_ONLY(, i_VALRAW rmapped)) { \
        cx_result_t res = cx_memb(_insert_entry_)(self, rkey); \
        if (res.inserted) { \
            *cx_keyref(res.ref) = i_KEYFROM(rkey); \
            cx_MAP_ONLY(res.ref->second = i_VALFROM(rmapped);) \
        } \
        return res; \
    } \
\
    STC_INLINE void \
    cx_memb(_emplace_items)(Self* self, const cx_rawvalue_t arr[], size_t n) { \
        for (size_t i=0; i<n; ++i) cx_SET_ONLY( cx_memb(_emplace)(self, arr[i]); ) \
                                   cx_MAP_ONLY( cx_memb(_emplace)(self, arr[i].first, arr[i].second); ) \
    } \
\
    STC_INLINE cx_result_t \
    cx_memb(_insert)(Self* self, i_KEY key cx_MAP_ONLY(, i_VAL mapped)) { \
        cx_result_t res = cx_memb(_insert_entry_)(self, i_KEYTO(&key)); \
        if (res.inserted) {*cx_keyref(res.ref) = key; cx_MAP_ONLY( res.ref->second = mapped; )} \
        else              {i_KEYDEL(&key); cx_MAP_ONLY( i_VALDEL(&mapped); )} \
        return res; \
    } \
\
    cx_MAP_ONLY( \
        STC_INLINE cx_result_t \
        cx_memb(_insert_or_assign)(Self* self, i_KEY key, i_VAL mapped) { \
            cx_result_t res = cx_memb(_insert_entry_)(self, i_KEYTO(&key)); \
            if (res.inserted) res.ref->first = key; \
            else {i_KEYDEL(&key); i_VALDEL(&res.ref->second); } \
            res.ref->second = mapped; return res; \
        } \
    \
        STC_INLINE cx_result_t \
        cx_memb(_put)(Self* self, i_KEY key, i_VAL mapped) { \
            return cx_memb(_insert_or_assign)(self, key, mapped); \
        } \
    \
        STC_INLINE cx_result_t \
        cx_memb(_emplace_or_assign)(Self* self, i_KEYRAW rkey, i_VALRAW rmapped) { \
            cx_result_t res = cx_memb(_insert_entry_)(self, rkey); \
            if (res.inserted) res.ref->first = i_KEYFROM(rkey); \
            else i_VALDEL(&res.ref->second); \
            res.ref->second = i_VALFROM(rmapped); return res; \
        } \
    \
        STC_INLINE cx_mapped_t* \
        cx_memb(_at)(const Self* self, i_KEYRAW rkey) { \
            cx_iter_t it; \
            return &cx_memb(_find_it)(self, rkey, &it)->second; \
        }) \
\
    STC_INLINE cx_iter_t \
    cx_memb(_begin)(const Self* self) { \
        cx_iter_t it; it._d = self->nodes, it._top = 0; \
        it._tn = (cx_memb(_size_t)) _csmap_rep(self)->root; \
        if (it._tn) cx_memb(_next)(&it); \
        return it; \
    } \
\
    STC_INLINE cx_iter_t \
    cx_memb(_end)(const Self* self) {\
        return c_make(cx_iter_t){.ref = NULL}; \
    } \
\
    STC_INLINE cx_iter_t \
    cx_memb(_fwd)(cx_iter_t it, size_t n) { \
        while (n-- && it.ref) cx_memb(_next)(&it); \
        return it; \
    } \
\
    _c_implement_aatree(Self, C, i_KEY, i_VAL, i_CMP, \
                        i_VALDEL, i_VALFROM, i_VALTO, i_VALRAW, \
                        i_KEYDEL, i_KEYFROM, i_KEYTO, i_KEYRAW) \
    struct stc_trailing_semicolon

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
static struct csmap_rep _csmap_sentinel = {0, 0, 0, 0, 0};

#define _c_implement_aatree(Self, C, i_KEY, i_VAL, i_CMP, \
                            i_VALDEL, i_VALFROM, i_VALTO, i_VALRAW, \
                            i_KEYDEL, i_KEYFROM, i_KEYTO, i_KEYRAW) \
    STC_DEF Self \
    cx_memb(_init)(void) { \
        Self tree = {(cx_node_t *) _csmap_sentinel.nodes}; \
        return tree; \
    } \
\
    STC_DEF cx_value_t* \
    cx_memb(_front)(const Self* self) { \
        cx_node_t *d = self->nodes; \
        cx_memb(_size_t) tn = (cx_memb(_size_t)) _csmap_rep(self)->root; \
        while (d[tn].link[0]) tn = d[tn].link[0]; \
        return &d[tn].value; \
    } \
\
    STC_DEF cx_value_t* \
    cx_memb(_back)(const Self* self) { \
        cx_node_t *d = self->nodes; \
        cx_memb(_size_t) tn = (cx_memb(_size_t)) _csmap_rep(self)->root; \
        while (d[tn].link[1]) tn = d[tn].link[1]; \
        return &d[tn].value; \
    } \
\
    STC_DEF void \
    cx_memb(_reserve)(Self* self, size_t cap) { \
        struct csmap_rep* rep = _csmap_rep(self); \
        cx_memb(_size_t) oldcap = rep->cap; \
        if (cap > oldcap) { \
            rep = (struct csmap_rep*) c_realloc(oldcap ? rep : NULL, \
                                                sizeof(struct csmap_rep) + (cap + 1)*sizeof(cx_node_t)); \
            if (oldcap == 0) \
                memset(rep, 0, sizeof(struct csmap_rep) + sizeof(cx_node_t)); \
            rep->cap = cap; \
            self->nodes = (cx_node_t *) rep->nodes; \
        } \
    } \
\
    STC_DEF cx_memb(_size_t) \
    cx_memb(_node_new_)(Self* self, int level) { \
        size_t tn; struct csmap_rep *rep = _csmap_rep(self); \
        if (rep->disp) { \
            tn = rep->disp; \
            rep->disp = self->nodes[tn].link[1]; \
        } else { \
            if ((tn = rep->head + 1) > rep->cap) cx_memb(_reserve)(self, 4 + (tn*13 >> 3)); \
            ++_csmap_rep(self)->head; /* do after reserve */ \
        } \
        cx_node_t* dn = &self->nodes[tn]; \
        dn->link[0] = dn->link[1] = 0; dn->level = level; \
        return (cx_memb(_size_t)) tn; \
    } \
\
    STC_DEF cx_value_t* \
    cx_memb(_find_it)(const Self* self, i_KEYRAW rkey, cx_iter_t* out) { \
        cx_memb(_size_t) tn = _csmap_rep(self)->root; \
        cx_node_t *d = out->_d = self->nodes; \
        out->_top = 0; \
        while (tn) { \
            int c; cx_rawkey_t raw = i_KEYTO(cx_keyref(&d[tn].value)); \
            if ((c = i_CMP(&raw, &rkey)) < 0) \
                tn = d[tn].link[1]; \
            else if (c > 0) \
                { out->_st[out->_top++] = tn; tn = d[tn].link[0]; } \
            else \
                { out->_tn = d[tn].link[1]; return (out->ref = &d[tn].value); } \
        } \
        return (out->ref = NULL); \
    } \
\
    STC_DEF cx_iter_t \
    cx_memb(_lower_bound)(const Self* self, i_KEYRAW rkey) { \
        cx_iter_t it; \
        cx_memb(_find_it)(self, rkey, &it); \
        if (!it.ref && it._top) { \
            cx_memb(_size_t) tn = it._st[--it._top]; \
            it._tn = it._d[tn].link[1]; \
            it.ref = &it._d[tn].value; \
        } \
        return it; \
    } \
\
    STC_DEF void \
    cx_memb(_next)(cx_iter_t *it) { \
        cx_memb(_size_t) tn = it->_tn; \
        if (it->_top || tn) { \
            while (tn) { \
                it->_st[it->_top++] = tn; \
                tn = it->_d[tn].link[0]; \
            } \
            tn = it->_st[--it->_top]; \
            it->_tn = it->_d[tn].link[1]; \
            it->ref = &it->_d[tn].value; \
        } else \
            it->ref = NULL; \
    } \
\
    static cx_memb(_size_t) \
    cx_memb(_skew_)(cx_node_t *d, cx_memb(_size_t) tn) { \
        if (tn && d[d[tn].link[0]].level == d[tn].level) { \
            cx_memb(_size_t) tmp = d[tn].link[0]; \
            d[tn].link[0] = d[tmp].link[1]; \
            d[tmp].link[1] = tn; \
            tn = tmp; \
        } \
        return tn; \
    } \
    static cx_memb(_size_t) \
    cx_memb(_split_)(cx_node_t *d, cx_memb(_size_t) tn) { \
        if (d[d[d[tn].link[1]].link[1]].level == d[tn].level) { \
            cx_memb(_size_t) tmp = d[tn].link[1]; \
            d[tn].link[1] = d[tmp].link[0]; \
            d[tmp].link[0] = tn; \
            tn = tmp; \
            ++d[tn].level; \
        } \
        return tn; \
    } \
\
    STC_DEF cx_memb(_size_t) \
    cx_memb(_insert_entry_i_)(Self* self, cx_memb(_size_t) tn, const cx_rawkey_t* rkey, cx_result_t* res) { \
        cx_memb(_size_t) up[64], tx = tn; \
        cx_node_t* d = self->nodes; \
        int c, top = 0, dir = 0; \
        while (tx) { \
            up[top++] = tx; \
            i_KEYRAW raw = i_KEYTO(cx_keyref(&d[tx].value)); \
            if ((c = i_CMP(&raw, rkey)) == 0) {res->ref = &d[tx].value; return tn; } \
            dir = (c < 0); \
            tx = d[tx].link[dir]; \
        } \
        tx = cx_memb(_node_new_)(self, 1); d = self->nodes; \
        res->ref = &d[tx].value, res->inserted = true; \
        if (top == 0) return tx; \
        d[up[top - 1]].link[dir] = tx; \
        while (top--) { \
            if (top) dir = (d[up[top - 1]].link[1] == up[top]); \
            up[top] = cx_memb(_skew_)(d, up[top]); \
            up[top] = cx_memb(_split_)(d, up[top]); \
            if (top) d[up[top - 1]].link[dir] = up[top]; \
        } \
        return up[0]; \
    } \
\
    STC_DEF cx_result_t \
    cx_memb(_insert_entry_)(Self* self, i_KEYRAW rkey) { \
        cx_result_t res = {NULL, false}; \
        cx_memb(_size_t) tn = cx_memb(_insert_entry_i_)(self, (cx_memb(_size_t)) _csmap_rep(self)->root, &rkey, &res); \
        _csmap_rep(self)->root = tn; \
        _csmap_rep(self)->size += res.inserted; \
        return res; \
    } \
\
    STC_DEF cx_memb(_size_t) \
    cx_memb(_erase_r_)(cx_node_t *d, cx_memb(_size_t) tn, const cx_rawkey_t* rkey, int *erased) { \
        if (tn == 0) return 0; \
        i_KEYRAW raw = i_KEYTO(cx_keyref(&d[tn].value)); \
        cx_memb(_size_t) tx; int c = i_CMP(&raw, rkey); \
        if (c != 0) \
            d[tn].link[c < 0] = cx_memb(_erase_r_)(d, d[tn].link[c < 0], rkey, erased); \
        else { \
            if (!(*erased)++) cx_memb(_value_del)(&d[tn].value); \
            if (d[tn].link[0] && d[tn].link[1]) { \
                tx = d[tn].link[0]; \
                while (d[tx].link[1]) \
                    tx = d[tx].link[1]; \
                d[tn].value = d[tx].value; /* move */ \
                raw = i_KEYTO(cx_keyref(&d[tn].value)); \
                d[tn].link[0] = cx_memb(_erase_r_)(d, d[tn].link[0], &raw, erased); \
            } else { /* unlink node */ \
                tx = tn; \
                tn = d[tn].link[ d[tn].link[0] == 0 ]; \
                /* move it to disposed nodes list */ \
                struct csmap_rep *rep = c_container_of(d, struct csmap_rep, nodes); \
                d[tx].link[1] = (cx_memb(_size_t)) rep->disp; \
                rep->disp = tx; \
            } \
        } \
        tx = d[tn].link[1]; \
        if (d[d[tn].link[0]].level < d[tn].level - 1 || d[tx].level < d[tn].level - 1) { \
            if (d[tx].level > --d[tn].level) \
                d[tx].level = d[tn].level; \
                            tn = cx_memb(_skew_)(d, tn); \
            tx = d[tn].link[1] = cx_memb(_skew_)(d, d[tn].link[1]); \
                 d[tx].link[1] = cx_memb(_skew_)(d, d[tx].link[1]); \
                            tn = cx_memb(_split_)(d, tn); \
                 d[tn].link[1] = cx_memb(_split_)(d, d[tn].link[1]); \
        } \
        return tn; \
    } \
\
    STC_DEF int \
    cx_memb(_erase)(Self* self, i_KEYRAW rkey) { \
        int erased = 0; \
        cx_memb(_size_t) root = cx_memb(_erase_r_)(self->nodes, (cx_memb(_size_t)) _csmap_rep(self)->root, &rkey, &erased); \
        return erased ? (_csmap_rep(self)->root = root, --_csmap_rep(self)->size, 1) : 0; \
    } \
\
    STC_DEF cx_iter_t \
    cx_memb(_erase_at)(Self* self, cx_iter_t it) { \
        cx_rawkey_t raw = i_KEYTO(cx_keyref(it.ref)), nxt; \
        cx_memb(_next)(&it); \
        if (it.ref) nxt = i_KEYTO(cx_keyref(it.ref)); \
        cx_memb(_erase)(self, raw); \
        if (it.ref) cx_memb(_find_it)(self, nxt, &it); \
        return it; \
    } \
\
    STC_DEF cx_iter_t \
    cx_memb(_erase_range)(Self* self, cx_iter_t it1, cx_iter_t it2) { \
        if (!it2.ref) { while (it1.ref) it1 = cx_memb(_erase_at)(self, it1); \
                        return it1; } \
        cx_key_t k1 = *cx_keyref(it1.ref), k2 = *cx_keyref(it2.ref); \
        cx_rawkey_t r1 = i_KEYTO(&k1); \
        for (;;) { \
            if (memcmp(&k1, &k2, sizeof k1) == 0) return it1; \
            cx_memb(_next)(&it1); k1 = *cx_keyref(it1.ref); \
            cx_memb(_erase)(self, r1); \
            cx_memb(_find_it)(self, (r1 = i_KEYTO(&k1)), &it1); \
        } \
    } \
\
    static cx_memb(_size_t) \
    cx_memb(_clone_r_)(Self* self, cx_node_t* src, cx_memb(_size_t) sn) { \
        if (sn == 0) return 0; \
        cx_memb(_size_t) tx, tn = cx_memb(_node_new_)(self, src[sn].level); \
        cx_memb(_value_clone)(&self->nodes[tn].value, &src[sn].value); \
        tx = cx_memb(_clone_r_)(self, src, src[sn].link[0]); self->nodes[tn].link[0] = tx; \
        tx = cx_memb(_clone_r_)(self, src, src[sn].link[1]); self->nodes[tn].link[1] = tx; \
        return tn; \
    } \
    STC_DEF Self \
    cx_memb(_clone)(Self tree) { \
        Self clone = cx_memb(_with_capacity)(_csmap_rep(&tree)->size); \
        cx_memb(_size_t) root = cx_memb(_clone_r_)(&clone, tree.nodes, (cx_memb(_size_t)) _csmap_rep(&tree)->root); \
        _csmap_rep(&clone)->root = root; \
        _csmap_rep(&clone)->size = _csmap_rep(&tree)->size; \
        return clone; \
    } \
\
    STC_DEF void \
    cx_memb(_del_r_)(cx_node_t* d, cx_memb(_size_t) tn) { \
        if (tn) { \
            cx_memb(_del_r_)(d, d[tn].link[0]); \
            cx_memb(_del_r_)(d, d[tn].link[1]); \
            cx_memb(_value_del)(&d[tn].value); \
        } \
    } \
    STC_DEF void \
    cx_memb(_del)(Self* self) { \
        if (_csmap_rep(self)->root) { \
            cx_memb(_del_r_)(self->nodes, (cx_memb(_size_t)) _csmap_rep(self)->root); \
            c_free(_csmap_rep(self)); \
        } \
    }

#endif
#endif
