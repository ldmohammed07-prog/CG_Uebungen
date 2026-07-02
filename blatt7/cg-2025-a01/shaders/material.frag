#version 150

// Teilaufgabe 1.2
// Das Framework (Material::bind) setzt die diffuse Materialfarbe als vec4-Uniform "k_diff".
// Deshalb muss sie hier auch als vec4 deklariert werden (eine vec3-Deklaration passt nicht
// zu glUniform4f und die Uniform bliebe ungesetzt -> schwarze Oberflaeche).
uniform vec4 k_diff;

out vec4 out_col;

void main() {
	// diffuse Materialfarbe auf allen Oberflaechen anzeigen
	out_col = vec4(k_diff.rgb, 1.0);
}
