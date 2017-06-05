/*
 * avlc: another vhdl language compiler
 * Copyright (C) 2016 Rishabh Rawat
 *
 * avlc is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef AVLC_TYPES_H
#define AVLC_TYPES_H


//with Interfaces;

#include <exception>
#include <stdexcept>
#include "strong_typedef.hpp"

// A tri state type.
enum class triStateType {
	Unknown,
	False,
	True
};

class subtypeException: std::runtime_error {
public:
	subtypeException(const std::string& __arg)
			:runtime_error(__arg) { }
};

template<typename T, T min, T max>
class subtype {
    using className = subtype;
	T val;
public:
	subtype(T value) {
		if (value>max || value<min)
			throw subtypeException("out of subtype range");
        val = value;
	}

	bool operator<(const className& rhs) const {
		return val<rhs.val;
	}

	bool operator>(const className& rhs) const {
		return rhs<rhs.val;
	}

	bool operator<=(const className& rhs) const {
		return rhs<=rhs.val;
	}

	bool operator>=(const className& rhs) const {
		return rhs>=rhs.val;
	}

	bool operator==(const className& rhs) const {
		return val==rhs.val;
	}

	bool operator!=(const className& rhs) const {
		return val!=rhs.val;
	}

	subtype operator+(const className& rhs) const {
		return val+rhs.val;
	}

	subtype operator-(const className& rhs) const {
		return val-rhs.val;
	}

	subtype operator*(const className& rhs) const {
		return val*rhs.val;
	}

	subtype operator/(const className& rhs) const {
		return val/rhs.val;
	}
};

using Int32 = subtype<int32_t, INT32_MIN, INT32_MAX>;
using Nat32 = subtype<int32_t, 0, INT32_MAX>;
using Nat8 = subtype<uint8_t, 0, 255>;
using Uns32 = subtype<uint32_t, 0, UINT32_MAX>;
using Fp64 = double;

using Iir_Int32 = int32_t;
using Iir_Int64 = int64_t;
using Iir_Fp64 = double;
using Iir_Index32 = subtype<int32_t, 0, INT32_MAX>;

//using Name_Id = subtype<int32_t, 0, INT32_MAX>; // Null_Identifier == 0

//  PSL Node.
using PSL_Node = int32_t;

//  PSL NFA
using PSL_NFA  = int32_t;
#endif //AVLC_TYPES_H
