#version 150
#define USE_PHONG_SHADING

// TODO
// Teilaufgabe 3
// Uniforms und Input-Attribute

out vec4 out_col;
// --- WORLDSPACE-VARIANTE (aktiv) ---
in vec3 pos_ws;
in vec3 n_ws;
// --- EYESPACE-VARIANTE (Alternative): Position/Normale in Kamerakoordinaten ---
//in vec3 pos_es;
//in vec3 n_es;
//uniform mat4 view;   // noetig, um die Licht-Uniforms (Worldspace) in den Eyespace zu bringen
// Licht
uniform vec3 cam_pos;
uniform vec3 pointlight_pos;
uniform vec3 pointlight_col;
uniform float pointlight_scale;
uniform vec3 dirlight_dir;
uniform vec3 dirlight_col;
uniform float dirlight_scale;
// Material
uniform vec4 k_diff;
uniform vec4 k_spec;
uniform vec4 k_amb;

void main() {
	// TODO
	// Teilaufgabe 3
	// Phong-Beleuchtung für zwei Lichtquellen mit Diffuse und Spekularteil.

	float shininess = 32.0;   // spekularer Exponent, frei waehlbar

	vec3 N = normalize(n_ws);
	vec3 V = normalize(cam_pos - pos_ws);   // Richtung zur Kamera (Schritt 3)

	// --- Directional Light ---
	vec3 L = normalize(-dirlight_dir);      // Richtung ZUM Licht = Gegenrichtung
	// Schritt 1: Diffuse
	vec3 dir_diff = k_diff.rgb * dirlight_col * dirlight_scale * max(dot(N, L), 0.0);
	// Schritt 3: Specular
	vec3  Rd       = reflect(-L, N);        // gespiegelter Lichtstrahl
	float dir_s    = (dot(N, L) > 0.0) ? pow(max(dot(Rd, V), 0.0), shininess) : 0.0;
	vec3  dir_spec = k_spec.rgb * dirlight_col * dirlight_scale * dir_s;

	// --- Point Light (mit Distance-Falloff) ---
	vec3  to_light = pointlight_pos - pos_ws;   // Vektor vom Fragment zum Licht
	vec3  Lp       = normalize(to_light);
	float dist     = length(to_light);
	float atten    = 1.0 / (1.0 + 0.000002 * dist * dist);   // Falloff, frei waehlbar
	// Schritt 2: Diffuse
	vec3  pt_diff  = k_diff.rgb * pointlight_col * pointlight_scale * atten * max(dot(N, Lp), 0.0);
	// Schritt 3: Specular
	vec3  Rp       = reflect(-Lp, N);
	float pt_s     = (dot(N, Lp) > 0.0) ? pow(max(dot(Rp, V), 0.0), shininess) : 0.0;
	vec3  pt_spec  = k_spec.rgb * pointlight_col * pointlight_scale * atten * pt_s;

	// Schritt 4: Ambient (konstante Grundhelligkeit gegen tiefschwarze Schatten)
	vec3 ambient = k_amb.rgb;

	out_col = vec4(ambient + dir_diff + dir_spec + pt_diff + pt_spec, 1.0);


	// ================================================================
	// EYESPACE-VARIANTE (Alternative zum obigen Worldspace-Code)
	// Zum Testen: oben pos_ws/n_ws-Berechnung durch diese ersetzen und
	// im Vertex-Shader pos_es/n_es aktivieren.
	//
	// Kernidee: Im Eyespace liegt die KAMERA im Ursprung (0,0,0).
	// -> Antwort auf die Aufgabenfrage "Richtung zur Kamera?":
	//    V = normalize(-pos_es);   (cam_pos wird NICHT mehr gebraucht)
	// Die Licht-Uniforms sind im Worldspace und muessen mit view in den
	// Eyespace transformiert werden.
	//
	//	float shininess = 32.0;
	//
	//	vec3 N = normalize(n_es);
	//	vec3 V = normalize(-pos_es);          // Kamera im Ursprung
	//
	//	// Directional Light: nur Richtung transformieren (mat3(view))
	//	vec3 L = normalize(-mat3(view) * dirlight_dir);
	//	vec3 dir_diff = k_diff.rgb * dirlight_col * dirlight_scale * max(dot(N, L), 0.0);
	//	vec3  Rd       = reflect(-L, N);
	//	float dir_s    = (dot(N, L) > 0.0) ? pow(max(dot(Rd, V), 0.0), shininess) : 0.0;
	//	vec3  dir_spec = k_spec.rgb * dirlight_col * dirlight_scale * dir_s;
	//
	//	// Point Light: Position als Punkt transformieren (view * vec4(pos,1))
	//	vec3  plpos_es = (view * vec4(pointlight_pos, 1.0)).xyz;
	//	vec3  to_light = plpos_es - pos_es;
	//	vec3  Lp       = normalize(to_light);
	//	float dist     = length(to_light);
	//	float atten    = 1.0 / (1.0 + 0.000002 * dist * dist);
	//	vec3  pt_diff  = k_diff.rgb * pointlight_col * pointlight_scale * atten * max(dot(N, Lp), 0.0);
	//	vec3  Rp       = reflect(-Lp, N);
	//	float pt_s     = (dot(N, Lp) > 0.0) ? pow(max(dot(Rp, V), 0.0), shininess) : 0.0;
	//	vec3  pt_spec  = k_spec.rgb * pointlight_col * pointlight_scale * atten * pt_s;
	//
	//	vec3 ambient = k_amb.rgb;
	//	out_col = vec4(ambient + dir_diff + dir_spec + pt_diff + pt_spec, 1.0);
	// ================================================================
}
