/* Projektionen von 2D Geometrie auf eine 1D Bildebene */

import homo2d;
unitsize(1cm);

// Zeichne ein Koordinatensystem (wobei x und y je `ticks' lang sind)
void draw_frame(transform space, int ticks, pen color) {
	draw(space*((0,0)--(ticks,0)), color, Arrow);
	draw(space*((0,0)--(0,ticks)), color, Arrow);
	for (int i = 1; i < ticks; ++i) {
		draw(space*((i,-0.05)--(i,0.05)), color);
		draw(space*((-0.05,i)--(0.05,i)), color);
	}
	label("$x$", space*(ticks+0.2,0), color);
	label("$y$", space*(0,ticks+0.2), color);
}

// Zeichne den Worldspace
draw_frame(identity, 10, black);
label("Worldspace", (5,0), align=S);

// Zeichne den Eyespace (Blickrichtung ist hier im Eyespace -y
transform eyespace = shift(2,7)*rotate(+45);
draw_frame(eyespace, 3, heavygreen);
label("Eyespace", eyespace*(0,0), heavygreen, align=S);

// Definiere den Sichtbereich im Eyespace
real x_min = -2, x_max = 2;
real near = 1, far = 10;
// Zeichne diesen Sichtbereich ein
draw(eyespace*((x_min,-near)--(x_min,-far)--(x_max,-far)--(x_max,-near)--cycle), dotted + heavygreen);

// Homogene Matrix für Eyespace
// siehe homo2d.asy
M to_eyespace = matrix(inverse(eyespace));

// Ein Polygon, hier können Sie gerne weitere hinzufügen, verschiedene transformieren etc.
// Verschaffen sich sich damit (und mit den folgenden drei Anzeigen) einen Überblick, was genau passiert!
path poly = (4,7) -- (7,7) -- (5,3) -- cycle;

// Polygon im Worldspace zeichnen
draw(poly);

// Polygon im Eyespace zeichnen (d.h. das schwarze Koordinatensystem wird nun als Eyespace verstanden)
// Vollziehen Sie diese Abbildung genau nach!
draw(transform_path(to_eyespace, poly), orange);

// Polygon im Worldspace zeichnen (hier: in Eyespace transformieren, dann aber
// Eyespace Matrix wieder verwenden um es "für uns" im dargestellten Eyespace
// anzuzeigen)
draw(eyespace*transform_path(to_eyespace, poly), dashed+heavygreen);


