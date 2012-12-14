
#define D3DXSHADER_DEBUG
#define D3DXSHADER_SKIPOPTIMIZATION

#ifndef MAX_LIGHT_COUNT
	#define MAX_LIGHT_COUNT (4)
#endif

struct VERTEX_IN 
{ 
	float4 Position : POSITION;
	float4 Normal : NORMAL;
	float2 Texture : TEXCOORD0;
};

struct VERTEX_OUT
{
	float4 Position : POSITION;
	float3 Normal : TEXCOORD0;
	float2 Texture : TEXCOORD1;

	float3 EyeDir : TEXCOORD2;
	float Depth : TEXCOORD3;
	float3 LightDir[MAX_LIGHT_COUNT] : TEXCOORD4;
};


float3 CalcLightDir(float3 worldPos, float4 lightPos)
{	
	return lightPos.xyz - worldPos*lightPos.w;
}

float Fog(float2 c)
{
	const float2 FOG_START = float2(10000, 300);
	const float2 FOG_END = float2(100000, 3000);
	const float2 FOG_EXPONENT = float2(0.2, 0.2);

	c = saturate((c-FOG_START)/(FOG_END-FOG_START));
	c = pow(c, FOG_EXPONENT);
	return length(c);
}

VERTEX_OUT VS_Phong(in VERTEX_IN v, 
					   uniform float4x4 world,
					   uniform float4x4 worldView,
					   uniform float4x4 worldViewProj,
					   uniform float4x4 worldIT,
					   uniform float4 cameraPosition,
					   uniform float4 lightPositions[MAX_LIGHT_COUNT],
					   uniform float lightCount
					   ) 
{
	VERTEX_OUT res = (VERTEX_OUT)0;
	
	res.Position = mul(v.Position, worldViewProj); 
	res.Normal = normalize(mul(v.Normal, worldIT).xyz);

	float3 worldPos = mul(v.Position, world);
	res.EyeDir = normalize(cameraPosition - worldPos);
	//res.Texture = (float2)lightCount;
	int cnt = (int)lightCount;
	for (int i=0;i<cnt;i++)
	{
		res.LightDir[i] = CalcLightDir(worldPos, lightPositions[i]);
	}
	
	res.Texture = v.Texture;
	
	res.Depth = -mul(v.Position, worldView).z;
	//float2 c = float2(-mul(v.Position, worldView).z, worldPos.y);
	//res.Depth = Fog(c);
	return res;
}

float Step(float l, float eps, float A, float B)
{
	return A + smoothstep(B-eps, B+eps, l)*(B-A);
}


float QuantizeDiffuse(float l)
{
	const float A = 0.1, B = 0.3, C = 0.6, D = 1.0;
	float eps = fwidth(l);
	eps*=2;
		
	//return Step(l, eps, A, B);

	if (l>D-eps) return Step(l, eps, C, D);
	else if (l>C-eps) return Step(l, eps, B, C);
	else if (l>B-eps) return Step(l, eps, A, B);
	else return Step(l, eps, 0, A);

	/*if (l < A) return 0;
	else if (l < B) return A;
	else if (l < C) return B;
	else if (l < D) return C;
	else return D;*/
}

float QuantizeSpecular(float l)
{
	float eps = fwidth(l);
	eps *= 2;

	return smoothstep(0.5-eps, 0.5+eps, l);
}

float4 CalcDiffuse(float3 lightDir,
				  float3 n,
				  float4 diffuseLight,
				  float4 lightAttenuation)
{
	float lightDistance = length(lightDir);
	float3 L = lightDir / lightDistance;

	float luminance = 1 / (lightAttenuation.y + lightDistance*lightAttenuation.z + lightDistance*lightDistance*lightAttenuation.w);
	luminance *=  max(0, dot(n, L));
	luminance = QuantizeDiffuse(luminance);
	float4 difResult =  diffuseLight * luminance;
	return difResult;
}

void AddDiffuseSpecular(float3 lightDir,
						 float3 n,
						 float3 eyedir,
						 float4 diffuseLight,
						 float4 specularLight,
						 float4 lightAttenuation,
						 float shininess,
						 bool toonshade,
					     
						 inout float4 diffuse,
						 inout float4 specular
						 )
{
	float lightDistance = length(lightDir);
	float3 L = lightDir / lightDistance;
	float3 h = L+eyedir;
	h = normalize(h);
	
	float luminance = 1 / (lightAttenuation.y + lightDistance*lightAttenuation.z + lightDistance*lightDistance*lightAttenuation.w);

	float lumD = luminance * saturate(dot(n, L));
	float lumS = pow(saturate(dot(n,h)), shininess);
	
	if (toonshade)
	{
		//lumD = QuantizeDiffuse(lumD);
		lumS = QuantizeSpecular(lumS);
	}

	diffuse += lumD*diffuseLight;
	specular+=lumS*specularLight;
}


void SumDiffuseSpecularForAllLights(VERTEX_OUT v,
						float4 lightDiffuse[MAX_LIGHT_COUNT],
						float4 lightSpecular[MAX_LIGHT_COUNT],
						float4 lightAttenuation[MAX_LIGHT_COUNT],
						float shininess,
						float lightCount,
						bool toonshade,
						out float4 difSum,
						out float4 specSum)
{
	difSum = (float4)0;
	specSum = (float4)0;
	for (int i=0;i<lightCount;i++)
	{
		AddDiffuseSpecular(v.LightDir[i], v.Normal, v.EyeDir,
							lightDiffuse[i],
							lightSpecular[i],
							lightAttenuation[i],
							shininess,
							toonshade,
							difSum, specSum);
	}
	difSum /= lightCount;
}


float4 PS_Phong(VERTEX_OUT v,
				  uniform float4 ambient,
				  uniform float4 ambientLight,
				  uniform float4 diffuse,
				  uniform float4 specular,
				  uniform float shininess,

				  uniform float4 lightDiffuse[MAX_LIGHT_COUNT],
				  uniform float4 lightSpecular[MAX_LIGHT_COUNT],
				  uniform float4 lightAttenuation[MAX_LIGHT_COUNT],
				  uniform float lightCount
				  ) : COLOR0
{
	float4 difSum, specSum;
	SumDiffuseSpecularForAllLights(v,lightDiffuse,lightSpecular,lightAttenuation,shininess,lightCount, false, difSum, specSum);

	float4 c = (ambient*ambientLight + difSum*diffuse + specSum*specular);
	c.w = v.Depth;
	return c;
}

float4 PS_PhongTextured(VERTEX_OUT v,
				  uniform sampler2D tex : register(s0),
				  uniform float4 ambient,
				  uniform float4 ambientLight,
				  uniform float4 diffuse,
				  uniform float4 specular,
				  uniform float shininess,

				  uniform float4 lightDiffuse[MAX_LIGHT_COUNT],
				  uniform float4 lightSpecular[MAX_LIGHT_COUNT],
				  uniform float4 lightAttenuation[MAX_LIGHT_COUNT],
				  uniform float lightCount
				  ) : COLOR0
{
	float4 difSum, specSum;
	SumDiffuseSpecularForAllLights(v,lightDiffuse,lightSpecular,lightAttenuation,shininess,lightCount, false, difSum, specSum);
	
	float4 c = tex2D(tex, v.Texture);
	c = c*(ambient*ambientLight + difSum*diffuse) + specSum*specular;
	c.w = v.Depth;
	return c;
}

float4 PS_PhongToonshade(VERTEX_OUT v,
				  uniform float4 ambient,
				  uniform float4 ambientLight,
				  uniform float4 diffuse,
				  uniform float4 specular,
				  uniform float shininess,

				  uniform float4 lightDiffuse[MAX_LIGHT_COUNT],
				  uniform float4 lightSpecular[MAX_LIGHT_COUNT],
				  uniform float4 lightAttenuation[MAX_LIGHT_COUNT],
				  uniform float lightCount
				  ) : COLOR0
{
	float4 difSum, specSum;
	SumDiffuseSpecularForAllLights(v,lightDiffuse,lightSpecular,lightAttenuation,shininess,lightCount, true, difSum, specSum);
	
	return (ambient*ambientLight + difSum*diffuse + specSum*specular)*float4(1,1,1, 0.1);
}



float4 PS_PhongToonshadeTextured(VERTEX_OUT v,
				  uniform sampler2D tex : register(s0),
				  uniform float4 ambient,
				  uniform float4 ambientLight,
				  uniform float4 diffuse,
				  uniform float4 specular,
				  uniform float shininess,

				  uniform float4 lightDiffuse[MAX_LIGHT_COUNT],
				  uniform float4 lightSpecular[MAX_LIGHT_COUNT],
				  uniform float4 lightAttenuation[MAX_LIGHT_COUNT],
				  uniform float lightCount
				  ) : COLOR0
{
	float4 difSum, specSum;
	SumDiffuseSpecularForAllLights(v,lightDiffuse,lightSpecular,lightAttenuation,shininess,lightCount, true, difSum, specSum);
	
	float4 texel = tex2D(tex, v.Texture);
	return texel*(ambient*ambientLight + difSum*diffuse) + specSum*specular;
}