#version 460

#define MAX_LIGHT_COUNT 3

uniform struct LightInfo 
{
	// Generic
	vec3 PositionWS;
	vec3 Color;
	
    float DistanceFalloffExp;

	// Converts vector from world space to this light's view space
	mat4 WorldToLightMatrix;
	mat4 ProjectionMatrix;

	sampler2DShadow ShadowTexture;
	
	// Attenuation factors
	// XYZ: 1 / radius (for omni)
	// XY: 1 / tan(fov), Z: 1 / distance (for spot)
	vec3 InverseScale;

	// Omni

	// Spot
    float ConeHotspot;
    float ConeHotspotInverse;
    float ConeFalloffExp;
} Light[MAX_LIGHT_COUNT];

uniform int LightCount;

uniform struct MaterialInfo 
{
    //layout(binding = 0) sampler2D AlbedoTexture;
    vec3 Albedo;

    //layout(binding = 1) sampler2D RoughnessTexture;
    float Roughness;

    //layout(binding = 2) sampler2D MetallicTexture;
    float Metallic;

    //layout(binding = 3) sampler2D EmissiveTexture;
    vec3 Emissive;

    //layout(binding = 4) sampler2D AOTexture;
    //vec3 AO;
} Material;

// World space vertex position, normal & view vector
in vec3 VertexPositionWS;
in vec3 NormalWS;
in vec3 ViewVectorWS;
in vec3 LightVectorWS[MAX_LIGHT_COUNT];
in vec3 VertexPositionLS[MAX_LIGHT_COUNT];
in vec4 ShadowTC[MAX_LIGHT_COUNT];

layout (location = 0) out vec4 FragColor;

//
// Light attenuation
//

// 'p' - position in unit light space
float distanceAttenuation(vec3 p, float e)
{
	return pow(max(1.0 - dot(p, p), 0.0), e);
}

// 'p' - position in unit light space times fov factor
// 'h' - hotspot
// 'i' - 1 / (1 - h)
// 'e' - spot falloff exponent
float coneAttenuation(vec3 p, float h, float i, float e)
{
	float t = max(-p.z, 0.0);
	float c = length(p.xy);
	c = clamp((c / t - h) * i, 0.0, 1.0);

	return pow(1.0 - c * c, e);
}


#define PI 3.14159265358979323846

vec3 Diffuse_Lambert(vec3 color)
{
	return color * (1.0 / PI);
}

float SpecularD_GGX (float roughness, float NH)
{
	float m = roughness * roughness;
	float m2 = m * m;
	float d = (NH * m2 - NH) * NH + 1.0;

	return m2 / (PI * d * d);
}

float SpecularG_SmithApprox(float roughness, float NV, float NL)
{
	float a = roughness * roughness;
	float GV = NL * (NV * (1.0 - a) + a);
	float GL = NV * (NL * (1.0 - a) + a);

	return 0.5 / (GV + GL);
}

vec3 SpecularF_Schlick(vec3 color, float VH)
{
	float f = pow(1.0 - VH, 5.0);

	// Clamp reflectance to a minimum of 2% of the "brightest" green channel.
	return mix(color, clamp(50.0 * color.ggg, 0.0, 1.0), f);
}

//
// xyz - BRDF result
// w - N.L, for possible further fragment culling
//
vec4 directBRDF(vec3 diffuseColor, vec3 specularColor, float roughness, vec3 normal, vec3 lightVec, vec3 viewVec)
{
	float NL = clamp(dot(normal, lightVec), 0.0, 1.0);

	if (NL > 0.0)
	{
		NL *= clamp(NL * 6.0 - 0.2, 0.0, 1.0);

		//if (NL > 0.0)
		//{
			float NV = abs(dot(normal, viewVec)) + 0.00001;

			vec3 halfVec = normalize(lightVec + viewVec);
			float NH = clamp(dot(normal, halfVec), 0.0, 1.0);
			float VH = clamp(dot(viewVec, halfVec), 0.0, 1.0);

			vec3 diffuse = Diffuse_Lambert(diffuseColor);
			//vec3 diffuse = Diffuse_Burley(diffuseColor, roughness, NV, NL, VH);
			//vec3 diffuse = Diffuse_OrenNayar(diffuseColor, roughness, NV, NL, VH);

			//float D = SpecularD_Blinn(roughness, NH);
			//float D = SpecularD_Beckmann(roughness, NH);
			float D = SpecularD_GGX(roughness, NH);

			//float G = SpecularG_Neumann(NV, NL);
			//float G = SpecularG_Kelemen(VH);
			//float G = SpecularG_Schlick(roughness, NV, NL);
			//float G = SpecularG_Smith(roughness, NV, NL);
			float G = SpecularG_SmithApprox(roughness, NV, NL);

			//vec3 F = SpecularF_Fresnel(specularColor, VH);
			vec3 F = SpecularF_Schlick(specularColor, VH);

			return vec4(NL * (diffuse + D * G * F), NL);
		//}
	}

	return vec4(0.0, 0.0, 0.0, NL);
}

//
// Main
//

void main()
{
    vec3 N = normalize(NormalWS);
    vec3 V = normalize(ViewVectorWS);


	// Material input		
	
	vec3 albedo = /*texture(u_albedoTexture, v_texCoord).xyz **/ Material.Albedo;
	albedo = pow(albedo, vec3(2.2));

	float roughness = clamp(/*texture(u_roughnessTexture, v_texCoord).x **/ Material.Roughness, 0.0, 1.0);
	float metallic = clamp(/*texture(u_metallicTexture, v_texCoord).x **/ Material.Metallic, 0.0, 1.0);

	// Another color map
	vec3 emissive = /*texture(u_emissiveTexture, v_texCoord).xyz **/ Material.Emissive;
	emissive = pow(emissive, vec3(2.2));

	// Ambient occlusion
	float ao = /*texture(u_aoTexture, v_texCoord).x*/ 1.0;

	vec3 F0 = mix(vec3(0.08), albedo, metallic);
	
	// Direct lighting
	
	vec3 direct = vec3(0.0);

	for (int i = 0; i < LightCount; i++)
	{
		// Skip if the fragment is behind the projection plane
		if (ShadowTC[i].z < 0.0)
			continue;
		
		// Shadow mapping		

		vec3 sc = ShadowTC[i].xyz / ShadowTC[i].w;

#if 1
		// Simple PCF
		float shadow = 0.0;

		vec2 texelSize = 1.0 / textureSize(Light[i].ShadowTexture, 0);
		
		for(int x = -1; x <= 1; x++)
		{
			for(int y = -1; y <= 1; y++)
			{
				vec3 tc = ShadowTC[i].xyz / ShadowTC[i].w;
				tc.xy += vec2(x, y) * texelSize;
				
				shadow += texture(Light[i].ShadowTexture, tc).x;
			}    
		}

		shadow *= (1.0 / 9.0);
#else
		
		float shadow = textureProj(Light[i].ShadowTexture, ShadowTC[i]).x;

		
#endif

		// Skip if completely in shadow
		if (shadow <= 0.0)
			continue;
	
		// Light calculations		

		
		float A = 0.5 * ao + 0.5;

		// Light attenuation 
		A *= distanceAttenuation(VertexPositionLS[i] * Light[i].InverseScale.zzz, Light[i].DistanceFalloffExp);
		A *= coneAttenuation(VertexPositionLS[i] * Light[i].InverseScale.zzz * vec3(Light[i].InverseScale.x, Light[i].InverseScale.y, 1.0), Light[i].ConeHotspot, Light[i].ConeHotspotInverse, Light[i].ConeFalloffExp);

		
		if (A <= 0.0)
			continue;

		vec3 L = normalize(LightVectorWS[i]);

#if 1
		// BRDF.
		vec4 brdf = directBRDF(albedo, F0, roughness, N, L, V);

		if (brdf.w <= 0.0)
			continue;

		// We should set light intensity of PI in the app to get reflected value of 1 on perpendicular surface, but we keep it here for simplicity.
		vec3 light = PI * brdf.xyz;
#else
		// Simple Blinn-Phong BRDF.
		vec3 H = normalize(L + V);
		float LdotN = max(dot(L, N), 0.0);

		if (LdotN <= 0.0)
			continue;

		float NdotH = pow(max(dot(H, N), 0.0), 16.0);
		
		vec3 light = (LdotN + NdotH) * albedo;
#endif

		float gi = sqrt(max(dot(L, N), 0.0)) * 0.0;

		//direct += Light[i].Color * A * (light + gi) * shadow;
		direct += pow(Light[i].Color, vec3(2.2)) * A * (light + gi) * shadow;
		//direct += shadow;
		//direct += vec3(sc.z);
	}

    FragColor = vec4(direct + emissive, 1.0);
	FragColor = pow(FragColor, vec4(1.0 / 2.2));
	//FragColor.xyz = albedo;
	//FragColor.xyz = gl_FragCoord.zzz;
}
