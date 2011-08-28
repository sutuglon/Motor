/*
OZCollide - Collision Detection Library
Copyright (C) 2006  Igor Kravtchenko

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Contact the author: igor@tsarevitch.org
*/

#ifndef OZCOLLIDE_VECTOR_H
#define OZCOLLIDE_VECTOR_H

#ifndef OZCOLLIDE_PCH
#include <ozcollide/ozcollide.h>
#endif

ENTER_NAMESPACE_OZCOLLIDE

template <class T> class Vector {

public:

	Vector()
	{
		mem_ = NULL;
		size_ = 0;
		capacity_ = 0;
	}

	~Vector()
	{
		clear();
	}

	void add(const T &_t)
	{
		if (size_ < capacity_) {
			mem_[size_] = _t;
			size_++;
			return;
		}

		resize(size_ + 1);
		mem_[size_ - 1] = _t;
	}

	void insert(const T &_t, int _i)
	{
		if (_i > size())
			return;
		
		if (_i == size()) {
			add(_t);
			return;
		}

		int oldSize = size_;
		resize(size_ + 1);

		for (int j = oldSize; j > _i; j--)
			mem_[j] = mem_[j - 1];
		mem_[_i] = _t;
	}

	void remove(int _index)
	{
		if (unsigned(_index) >= unsigned(size_))
			return;
		T *t = &mem_[_index];
		T *right = mem_ + size_;
		if (t + 1 < right)
			memcpy(t, t + 1, (right - t - 1) * sizeof(T));
		size_--;
	}

	void removeElement(const T _t)
	{
		while(true) {
			int siz = size();
			int i = 0;
			for ( ; i < siz; i++) {
				if (mem_[i] == _t) {
					remove(i);
					break;
				}
			}
			if (i == siz)
				break;
		}
	}

	void replaceElement(const T _t, const T _by)
	{
		for (int i = 0; i < size_; i++) {
			if (mem_[i] == _t)
				mem_[i] = _by;
		}
	}

	void copy(Vector &_v) const
	{
		_v.clear();
		_v.size_ = size_;
		_v.capacity_ = capacity_;
		_v.mem_ = (T*) malloc(_v.capacity_ * sizeof(T));
		memcpy(_v.mem_, mem_, _v.capacity_ * sizeof(T));
	}

	void resize(int _size)
	{
		if (_size > capacity_)
			reserve(_size * 2);
		size_ = _size;
	}

	void reserve(int _capacity)
	{
		T *old = mem_;
		mem_ = (T*) malloc(_capacity * sizeof(T));
		int toCopy = _capacity > capacity_ ? capacity_ : _capacity;
		memcpy(mem_, old, toCopy * sizeof(T));
		if (old)
			free(old);
		capacity_ = _capacity;
	}

	void grow(int _size)
	{
		resize(size_ + _size);
	}

	T* inc()
	{
		resize(size_ + 1);
		return &mem_[size_ - 1];
	}

	void clear()
	{
		if (mem_)
			free(mem_);
		mem_ = NULL;
		size_ = 0;
		capacity_ = 0;
	}

	int size() const
	{
		return size_;
	}

	int capacity() const
	{
		return capacity_;
	}

	T* mem() const
	{
		return mem_;
	}

	T& last() const
	{
		return mem_[size_ - 1];
	}

	bool isIncluded(T t) const
	{
		for (int i = 0; i < size(); i++) {
			if (mem_[i] == t)
				return true;
		}
		return false;
	}

	int getIndexBuffer(T t) const
	{
		for (int i = 0; i < size(); i++) {
			if (mem_[i] == t)
				return i;
		}
		return -1;
	}

	T& operator [] (int index) const
	{
		return mem_[index];
	}

private:
	T		*mem_;
	int		size_;
	int		capacity_;
};

LEAVE_NAMESPACE

#endif
