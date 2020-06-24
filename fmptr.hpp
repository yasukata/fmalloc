/*
 *
 * Copyright 2020 Kenichi Yasukata
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef _FMPTR_H
#define _FMPTR_H

#include <stdio.h>
#include <stdint.h>

extern __thread uint64_t __fm_addr_base;

template <class T>
class fm_ptr {
private:
	uint64_t off;
public:
	template <class U>
	U operator = (U p) {
		if (p) {
			this->off = (uint64_t) p - __fm_addr_base;
			return (U) (__fm_addr_base + this->off);
		} else {
			this->off = 0;
			return (U) 0;
		}
	}

	template <class U>
	operator U* () const {
		if (this->off) {
			return (U*) (__fm_addr_base + this->off);
		} else {
			return NULL;
		}
	}

	fm_ptr() : off(0) {
	}

	fm_ptr<T>* operator & ()  {
		return (fm_ptr<T>*) this;
	}

	T& operator * ()  {
		if (this->off)
			return *((T*) (__fm_addr_base + this->off));
		else
			return NULL;
	}

	T* operator -> () {
		if (this->off)
			return (T*) (__fm_addr_base + this->off);
		else
			return NULL;
	}

	fm_ptr<T>& operator = (fm_ptr<T>& other) {
		this->off = other.off;
		return *this;
	}

	explicit operator bool () {
		return (bool) (this->off != 0);
	}

#define op_overload(__op) \
	template <class U> \
	T* operator __op (U other) { \
		if (this->off) \
			return (T*) ((__fm_addr_base + this->off) __op (uint64_t) other); \
		else \
			return (T*) (0 __op other); \
	}
	op_overload(+)
	op_overload(-)
	op_overload(*)
	op_overload(/)
	op_overload(|)
	op_overload(^)
	op_overload(<<)
	op_overload(>>)
#undef op_overload

#define cmp_overload(__op) \
	bool operator __op (fm_ptr<T>& other) { \
		return (this->off __op other.off); \
	} \
	template <class U> \
	bool operator __op (U other) { \
		if (this->off == 0) \
			return ((uint64_t) (0) __op (uint64_t) other); \
		else \
			return ((uint64_t)((U) (__fm_addr_base + this->off)) __op (uint64_t) other); \
	}
	cmp_overload(==)
	cmp_overload(!=)
	cmp_overload(<)
	cmp_overload(<=)
	cmp_overload(>)
	cmp_overload(>=)
#undef cmp_overload

#define cast_overload(__op) \
	explicit operator __op () { \
		if (this->off) \
			return (__op) (__fm_addr_base + this->off); \
		else \
			return 0; \
	}
	cast_overload(short)
	cast_overload(int)
	cast_overload(long)
	cast_overload(long long)
	cast_overload(unsigned short)
	cast_overload(unsigned int)
	cast_overload(unsigned long)
	cast_overload(unsigned long long)
#undef cast_overload
} __attribute__((packed));

#endif
