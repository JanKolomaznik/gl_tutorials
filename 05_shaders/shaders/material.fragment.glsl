#version 430 core

const uint DIFFUSE = 1;
const uint SPECULAR = 1 << 1;
const uint BUMP = 1 << 2;
const uint PARALLAX = 1 << 3;
const uint AMBIENT_OCC = 1 << 4;
const uint SHADOW = 1 << 5;
const uint DEBUG = 1 << 7;


uniform uint configuration = DIFFUSE;

const vec3 lightColor = vec3(1.0, 1.0, 1.0);
const float lightPower = 1.0;
const float ambientMultiplier = 0.3;
//uniform vec3 ambientColor = vec3(0.0, 0.0, 0.0);
// uniform vec3 diffuseColor = vec3(0.6, 0.5, 0.0);
//const vec3 specColor = vec3(1.0, 1.0, 1.0);
//uniform vec3 specColor = vec3(0.0, 0.0, 0.0);
const float shininess = 100.0;

layout(binding = 0) uniform sampler2D u_diffuseTexture;
layout(binding = 1) uniform sampler2D u_specularTexture;
layout(binding = 2) uniform sampler2D u_normalTexture;
layout(binding = 3) uniform sampler2D u_displacementTexture;
layout(binding = 4) uniform sampler2D u_ambientOccTexture;
layout(binding = 6) uniform sampler2D u_shadowMap;
//layout(binding = 6) uniform sampler2DShadow u_shadowMap;

uniform mat4 u_modelMat;
uniform mat4 u_viewMat;
uniform mat4 u_projMat;
uniform mat3 u_normalMat;

const vec3 u_lightPosition = vec3(15.0, 20.0, -10.0);
//uniform vec3 u_lightPosition = vec3(10.0, 5.0, 5.0);
uniform vec3 u_viewPos;

uniform float u_numberOfParallaxLayers = 10;

in vec3 f_normal;
in vec3 f_position;
in vec2 f_texCoord;

out vec4 out_color;

//************************************************************
float phong_specular(
		in vec3 normal,
		in vec3 viewDir,
		in vec3 lightDir,
		in float shininess)
{
	vec3 reflectDir = reflect(-lightDir, normal);
	float specAngle = max(dot(reflectDir, viewDir), 0.0);

	float power = pow(specAngle, shininess/4.0);
	return power;
}

float step_lambertian(float lambertian, int steps) {
	return int(lambertian * steps + float(steps - 1)/steps) / float(steps);
}

vec3 phong(
		in vec3 ambientColor,
		in vec3 diffuseColor,
		in vec3 specColor,
		in vec3 normal,
		in vec3 viewDir,
		in vec3 lightColor,
		in vec3 lightDir,
		in float lightPower,
		in float shininess,
		bool useSpecular)
{
	float lambertian = max(dot(lightDir, normal), 0.0);
	float specular = 0.0;

	if(useSpecular && lambertian > 0.0) {
		specular = phong_specular(normal, viewDir, lightDir, shininess);
	}
	return ambientColor +
		(diffuseColor * lambertian * lightColor +
		specColor * specular * lightColor) * lightPower;
}

vec3 phong_toon(
		in vec3 ambientColor,
		in vec3 diffuseColor,
		in vec3 specColor,
		in vec3 normal,
		in vec3 viewDir,
		in vec3 lightColor,
		in vec3 lightDir,
		in float lightPower,
		in float shininess,
		bool useSpecular)
{
	float lambertian = max(dot(lightDir, normal), 0.0);
	float specular = 0.0;

	if(useSpecular && lambertian > 0.0) {
		lambertian = step_lambertian(lambertian, 5);
		specular = phong_specular(normal, viewDir, lightDir, shininess);
		if (specular > 0.5) {
			specular = 1.0;
		} else {
			specular = 0.0;
		}
	}
	return ambientColor +
		(diffuseColor * lambertian * lightColor +
		specColor * specular * lightColor) * lightPower;
}

//************************************************************

mat3 inverse3x3( mat3 M )
{
    // The original was written in HLSL, but this is GLSL,
    // therefore
    // - the array index selects columns, so M_t[0] is the
    //   first row of M, etc.
    // - the mat3 constructor assembles columns, so
    //   cross( M_t[1], M_t[2] ) becomes the first column
    //   of the adjugate, etc.
    // - for the determinant, it does not matter whether it is
    //   computed with M or with M_t; but using M_t makes it
    //   easier to follow the derivation in the text
    mat3 M_t = transpose( M );
    float det = dot( cross( M_t[0], M_t[1] ), M_t[2] );
    mat3 adjugate = mat3( cross( M_t[1], M_t[2] ),
                          cross( M_t[2], M_t[0] ),
                          cross( M_t[0], M_t[1] ) );
    return adjugate / det;
}

mat3 cotangent_frame( vec3 N, vec3 p, vec2 uv )
{
    // get edge vectors of the pixel triangle
    vec3 dp1 = dFdx( p );
    vec3 dp2 = dFdy( p );
    vec2 duv1 = dFdx( uv );
    vec2 duv2 = dFdy( uv );

    // solve the linear system
    vec3 dp2perp = cross( dp2, N );
    vec3 dp1perp = cross( N, dp1 );
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;

    // construct a scale-invariant frame
    float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
    return mat3( T * invmax, B * invmax, N );
}

vec3 perturb_normal(mat3 TBN, vec2 texcoord )
{
    vec3 map = texture2D( u_normalTexture, texcoord ).xyz;
    map = map * 255./127. - 128./127.;
    return normalize( TBN * map );
}

vec3 perturb_normal( vec3 N, vec3 V, vec2 texcoord )
{
    // assume N, the interpolated vertex normal and
    // V, the view vector (vertex to eye)
    vec3 map = texture2D( u_normalTexture, texcoord ).xyz;
    map = map * 255./127. - 128./127.;
    mat3 TBN = cotangent_frame( N, -V, texcoord );
    return normalize( TBN * map );
}

const vec2 scaleBias = vec2(0.01, 0.00);

vec2 parallax_map(mat3 TBN, vec3 V, vec2 texcoord)
{
	float height = texture2D(u_displacementTexture, texcoord).r;
	float v = height * scaleBias.x - scaleBias.y;
	vec3 eye = normalize(TBN * V);
	vec2 offset = - (eye.xy/eye.z * v);
	return offset;
}

vec2 steep_parallax_map(mat3 TBN, vec3 V, vec2 texCoords, float numLayers)
{
	if (numLayers < 1.1) {
		return parallax_map(TBN, V, texCoords);
	}
	// numLayers = number of depth layers
	// calculate the size of each layer
	float layerDepth = 1.0 / numLayers;
	// depth of current layer
	float currentLayerDepth = 0.0;

	vec3 eye = normalize(TBN * V);
	// the amount to shift the texture coordinates per layer (from vector P)
	vec2 P = eye.xy/eye.z * scaleBias.x;
	vec2 deltaTexCoords = P / numLayers;

	vec2  currentTexCoords     = texCoords;
	float currentDepthMapValue = texture2D(u_displacementTexture, texCoords).r;

	while(currentLayerDepth < currentDepthMapValue)
	{
	    // shift texture coordinates along direction of P
	    currentTexCoords -= deltaTexCoords;
	    // get depthmap value at current texture coordinates
	    currentDepthMapValue = texture2D(u_displacementTexture, currentTexCoords).r;
	    // get depth of next layer
	    currentLayerDepth += layerDepth;
	}
	return currentTexCoords - texCoords;
}


//************************************************************

void main() {
	vec3 toLight = normalize(u_lightPosition - f_position);
	vec3 toCamera = normalize(u_viewPos - f_position);
	vec2 updatedTexCoords = f_texCoord;
	vec3 updatedNormal = normalize(f_normal);

	mat3 TBN = cotangent_frame(updatedNormal, toCamera, updatedTexCoords);

	if (bool(configuration & PARALLAX)) {
		vec2 offset = parallax_map(TBN, toCamera, updatedTexCoords);
		// vec2 offset = steep_parallax_map(TBN, toCamera, updatedTexCoords, u_numberOfParallaxLayers);
		updatedTexCoords = updatedTexCoords + offset;
	}

	if (bool(configuration & BUMP)) {
		updatedNormal = perturb_normal(TBN, updatedTexCoords);
	}


	vec3 diffuseColor = texture(u_diffuseTexture, updatedTexCoords).rgb;
	vec3 specColor = texture(u_specularTexture, updatedTexCoords).rgb;
	vec3 ambient = ambientMultiplier * diffuseColor;

	if (bool(configuration & AMBIENT_OCC)) {
		ambient = ambientMultiplier * texture(u_ambientOccTexture, updatedTexCoords).r * diffuseColor;
	}
	if (configuration != DEBUG) {
		out_color = vec4(phong(
					ambient,
					diffuseColor,
					specColor,
					updatedNormal,
					toCamera,
					lightColor,
					toLight,
					lightPower,
					shininess,
					bool(configuration & SPECULAR)),
				1.0);
		if (bool(configuration & AMBIENT_OCC)) {
			out_color = out_color * texture(u_ambientOccTexture, updatedTexCoords).r;
		}
	} else {
		out_color = vec4(texture2D( u_normalTexture, updatedTexCoords ).xyz, 1.0);
	}
}

