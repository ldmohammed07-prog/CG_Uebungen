
// 3D Vertex - - - - - - - - - - - 

struct V {
	real x, y, z;
	string str(bool short = false) {
		if (short)
			return "[ " + format("%0.03f", x) + " " + format("%0.03f", y) + " " + format("%0.03f", z) + " ]";
		else
			return "[ " + format(x) + " " + format(y) + " " + format(z) + " ]";
	}
	// Position als Tripel
	triple t() { return (x, y, z); }
};

// "Construktor"
V vector(real x, real y, real z) {
	V v; v.x = x; v.y = y; v.z = z; return v;
}

V scale(V v, real r) {
	V s;
	s.x = v.x * r;
	s.y = v.y * r;
	s.z = v.z * r;
	return s;
}

V add(V a, V b) {
	V v;
	v.x = a.x + b.x;
	v.y = a.y + b.y;
	v.z = a.z + b.z;
	return v;
}

// Skalarprodukt
real dot(V a, V b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

// Länge
real length(V a) {
	return sqrt(dot(a,a));
}

V cross (V a, V b) {
	V v;
	v.x = a.y*b.z - a.z*b.y;
	v.y = a.z*b.x - a.x*b.z;
	v.z = a.x*b.y - a.y*b.x;
	return v;
}


// 4D Quaternion - - - - - - - - - - - 

struct Q {
	V v;
	real w;
	string str(bool short) {
		if (short)
			return "(" + v.str(short) + " " + format("%0.03f", w) + ")";
		else
			return "(" + v.str() + " " + format(w) + ")";
	}
};

// "Construktor"
Q quaternion(V v, real w) {
	Q q; q.v = v; q.w = w; return q;
}

// Norm
real n(Q q) {
	return dot(q.v, q.v) + q.w*q.w;
}

Q rotation(V axis, real angle) {
	assert(abs(dot(axis,axis) - 1.0) < 1e-7, "non-unit axis for rotation");
	assert(angle >= 0 && angle < 2*pi, "use \phi\in[0,2\pi] for rotation");
	Q q;
	q.v = scale(axis, sin(angle*0.5));
	q.w = cos(angle*0.5);
	return q;
}

Q mult(Q q, Q r) {
	Q p;
	p.v = add(cross(q.v, r.v), add(scale(q.v,r.w), scale(r.v,q.w)));
	p.w = q.w*r.w - dot(q.v, r.v);
	return p;
}

// Konjugiertes Quaternion
Q conj(Q q) {
	Q r;
	r.v.x = -q.v.x;
	r.v.y = -q.v.y;
	r.v.z = -q.v.z;
	r.w = q.w;
	return r;
}

Q scale(Q q, real s) {
	Q r;
	r.v = scale(q.v, s);
	r.w = q.w * s;
	return r;
}

Q add(Q q, Q r) {
	Q p;
	p.v = add(q.v, r.v);
	p.w = q.w + r.w;
	return p;
}


