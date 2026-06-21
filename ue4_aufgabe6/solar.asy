settings.tex = "pdflatex";

void draw_planet(transform t, real size, pen col) {
	fill(t*scale(size)*unitcircle, col);
	draw(t*scale(size)*unitcircle, col);
}

unitsize(0.1mm);

real sun_rad = 696;
real earth_rad = 6.3;
real earth_to_sun = 1500; // not to scale
real moon_rad = 1.7;
real moon_to_earth = 384;
real spaceelev_len = 35.8;

bool video_demo = false;
bool early_out = false;

if (video_demo) {
	sun_rad /= 10;
	earth_to_sun /= 10;
	moon_to_earth /= 10;
	spaceelev_len /= 3;
}

for (int d = 0; d < 365; d += 1) {

	for (int h = 0; h < 24; h+=2) {

		if (video_demo)
			filldraw(scale(400,400)*shift(-.5,-.5)*unitsquare, black, white+linewidth(1));
		else
			filldraw(scale(4000,4000)*shift(-.5,-.5)*unitsquare, black, white+linewidth(5));

		// This is the proper animation time
		real D = d + h/24;
		transform earth_pos = rotate(360*D/365) * shift(earth_to_sun, 0);
		transform moon_pos = earth_pos * rotate(360*D/27)*shift(moon_to_earth,0);

		draw_planet(identity, sun_rad, orange+yellow);
		draw_planet(earth_pos, earth_rad, blue+.4green);
		draw(earth_pos*rotate(h/24.0*360)*shift(earth_rad)*((0,0)--(spaceelev_len,0)), white);
		draw_planet(moon_pos, moon_rad, lightgray);

		shipout("solar_" + format("%03d", d) + format("%02d", h) + ".png");//, bbox(p,Fill));
		erase();
		if (early_out)
			exit();
	}
	if (d%10==0) write(".");
}

