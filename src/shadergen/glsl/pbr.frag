	/*
 Copyright (c) 2022 Tero Oinas

 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 of the Software, and to permit persons to whom the Software is furnished to do
 so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#line 24

const vec3 F0 = vec3(0.04); // Base reflectivity general dielectric surface

float distThrowbridgeReitzGGX(float rough, vec3 n, vec3 h) {
	const float a = rough * rough;
	const float a2 = a * a;
	const float ndh = clamp(dot(n, h), 0.0, 1.0);
	const float nhd2 = ndh * ndh;
	const float p = (nhd2 * (a2 - 1.0) + 1.0);
	
	return a2 / max(PI * p * p, 0.001);
}

float geometrySchlickGGX(vec3 n, vec3 v, float k)
{	
	const float ndv = clamp(dot(n, v), 0.0, 1.0);
	return ndv / (ndv * (1.0 - k) + k);
}

float geometrySmith(float rough, vec3 n, vec3 v, vec3 l)
{
	const float k = ((rough + 1.0) * (rough + 1.0)) / 8.0;
	return geometrySchlickGGX(n, v, k) * geometrySchlickGGX(n, l, k);
}

vec3 fresnelSchlick(vec3 h, vec3 v, vec3 f0)
{
	const float hdv = clamp(dot(h, v), 0.0, 1.0);
	return f0 + (1.0 - f0) * pow(clamp(1.0 - hdv, 0.0, 1.0), 5.0);
}

vec3 cookTorranceBRDF(vec3 albedo, float rough, float metallic, vec3 n, vec3 h, vec3 v, vec3 l, vec3 f0)
{
	const float d = distThrowbridgeReitzGGX(rough, n, h);
	const float g = geometrySmith(rough, n, v, l);
	const vec3 f = fresnelSchlick(h, v, f0);

	const float ndl = clamp(dot(n, l), 0.0, 1.0);
	const vec3 specular = (d * f * g) / 4.0; // max(4.0 * ndv * ndl, 0.0001);
	const vec3 df = (1.0 - f) * (1.0 - metallic);
	const vec3 diffuse = df * albedo / PI;

	return diffuse + specular;
}

vec3 radianceSpot(vec3 lightColor, vec3 lightV)
{
	const float d = length(lightV);
	const float att = 1.0 / (d * d);
	return lightColor * att;
}

vec4 pbr(vec3 albedo, vec3 normal, float metallic, float rough, float ao)
{	
	const vec3 viewDir = normalize(glob.viewPos.xyz - pos_w);
	const vec3 f0 = mix(F0, albedo, metallic);

	vec3 ro = vec3(0.0f);
	for(int i = 0; i < glob.numLights; i++) {
		const vec3 lightV = glob.lightPos[i].xyz - pos_w;
		const vec3 lightDir = normalize(lightV);
		const vec3 h = normalize(viewDir + lightDir);

		const vec3 brdf = cookTorranceBRDF(albedo, rough, metallic, normal, h, viewDir, lightDir, f0);
		const vec3 r = radianceSpot(glob.lightColor[i].rgb, lightV);
		const float ndl = clamp(dot(normal, lightDir), 0.0, 1.0);

		ro += brdf * r * ndl;
	}

  const vec3 ambient = glob.ambientColor.rgb * albedo * ao;
  const vec3 color = ambient + ro;
  return vec4(color, 1.0);
}

