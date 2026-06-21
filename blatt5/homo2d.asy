
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
	pair dehomo() { return (x/z, y/z); }
};

// "Construktor"
V vector(real x, real y, real z) {
	V v; v.x = x; v.y = y; v.z = z; return v;
}
V vector(pair p) {
	V v; v.x = p.x; v.y = p.y; v.z = 1; return v;
}

// Skalarprodukt
real dot(V a, V b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

struct M {
	V row1, row2, row3;
	V mult(V v) {
		return vector(dot(row1,v), dot(row2,v), dot(row3,v));
	}
};

M matrix(transform t) {
	M m;
	m.row1 = vector(t.xx, t.xy, t.x);
	m.row2 = vector(t.yx, t.yy, t.y);
	m.row3 = vector(0, 0, 1);
	return m;
}

M ortho(real xmin, real xmax, real near, real far) {
	M m;
	m.row1 = vector(2/(xmax-xmin), 0, -(xmax+xmin)/(xmax-xmin));
	m.row2 = vector(0, 2/(near-far), -(far+near)/(far-near));
	m.row3 = vector(0, 0, 1);
	return m;
}

M persp(real fov, real near, real far) {
	real h = near*tan(fov/2);
	real xmin = -h;
	real xmax = h;
	M m;
	m.row1 = vector(2*near/(xmax-xmin), (xmax+xmin)/(xmax-xmin), 0);
	m.row2 = vector(0, (near+far)/(near-far), 2*far*near/(near-far));
	m.row3 = vector(0, -1, 0);
	return m;
}

path transform_path(M m, path p) {
	path res = m.mult(vector(point(p, 0))).dehomo();
	for (int i = 1; i < length(p); ++i)
		res = res -- m.mult(vector(point(p, i))).dehomo();
	if (cyclic(p))
		res = res -- cycle;
	return res;
}

path transform_path(M m_left, M m_right, path p) {
	path res = m_left.mult(m_right.mult(vector(point(p, 0)))).dehomo();
	for (int i = 1; i < length(p); ++i)
		res = res -- m_left.mult(m_right.mult(vector(point(p, i)))).dehomo();
	if (cyclic(p))
		res = res -- cycle;
	return res;
}
