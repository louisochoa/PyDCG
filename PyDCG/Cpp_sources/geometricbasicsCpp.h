/*    PyDCG

   A Python library for Discrete and Combinatorial Geometry.

   Copyright (C) 2015 Ruy Fabila Monroy

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation version 2. 

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef GEOMETRICBASICSCPP_H_
#define GEOMETRICBASICSCPP_H_

#ifdef INT32
#define BIG_INT int64_t
#else
#define BIG_INT __int128_t
#endif

#include <cstdio>
#include <cstdlib>
#include <inttypes.h>
#include <vector>
#include <algorithm>

using std::vector;

static const short LEFT = -1;
static const short RIGHT = 1;
static const short COLLINEAR = 0;

struct punto
{
	long long x, y;
	int color;
	punto();
	punto(long long, long long);
	punto(long long, long long, int);
	bool operator==(const punto&) const;
	bool operator!=(const punto&) const;
	bool operator<(const punto& rhs) const;
	bool operator>(const punto& rhs) const;
};

struct triangulo
{
	punto a, b, c;
	triangulo();
	triangulo(punto, punto, punto);
	bool operator==(const triangulo&) const;
	bool operator!=(const triangulo&) const;
};

struct puntos_ordenados
{
	punto p;
	std::vector<punto> r;
	std::vector<punto> l;
	puntos_ordenados();
	puntos_ordenados(punto, std::vector<punto>, std::vector<punto>);
};

int turn(const punto&, const punto&, const punto&);
void orderandsplit(const std::vector<punto>&, std::vector<puntos_ordenados>&);
int general_position(std::vector<punto>&);

int turn(long p0[], long p1[], long p2[]);
int cmp_points(const void *qp, const void *rp);
void sort_around_point(long p[2], long pts[][2], int n);

void reverse_in_place(long pts[][2], int start, int end);
int concave(long p[2], long pts[][2], int n);
void shift(long pts[][2], int s, int n);
void print_pts(long pts[][2], int n);

#endif /* GEOMETRICBASICSCPP_H_ */
