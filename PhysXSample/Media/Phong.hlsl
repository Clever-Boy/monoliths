struct VERTEX_IN
{
	float4 position : POSITION;
	float4 normal	: NORMAL;
	float2 texCoord : TEXCOORD0;
};

struct VERTEX_OUT
{
	float4 hPos	: POSITION;
	float3 N 	: TEXCOORD0;
	float3 L      	: TEXCOORD1;
	float3 V      	: TEXCOORD2;
	float2 texCoord : TEXCOORD3;	
};

VERTEX_OUT vs(VERTEX_IN In,
		uniform float4x4 worldViewProj,
		uniform float4x4 worldView,
		uniform float4x4 worldViewIT,
		uniform float4 lightPos)
{
 VERTEX_OUT Out = (VERTEX_OUT) 0;

 Out.hPos = mul(worldViewProj, In.position);
 Out.texCoord = In.texCoord;
 Out.V = -mul(worldView, In.position).xyz;
 Out.L = lightPos.xyz + lightPos.w * Out.V;
 Out.N = mul(worldViewIT, In.normal).xyz;
 
 return Out;
}

float4 ps(VERTEX_OUT In,
	  uniform sampler2D colorTex : register(s0),
	  uniform float4 amientLightColor,
	  uniform float4 diffuseLightColor,
	  uniform float4 specularLightColor,
	  uniform float4 lightAttenuation,
	  uniform float4 ambientColor,
	  uniform float4 diffuseColor,
	  uniform float4 specularColor,
	  uniform float4 emissiveColor):COLOR0
{
	float3 N = normalize(In.N);
	float d = length(In.L);
	float3 L = In.L / d;
	float3 V = normalize(In.V);

	float4 texColor = tex2D(colorTex, In.texCoord);

	float attenuation = 1.0 / 
			(lightAttenuation.y +
			 lightAttenuation.z * d +
			 lightAttenuation.w * d * d);
	diffuseLightColor *= attenuation; 
	specularLightColor *= attenuation; 

	float ambient = amientLightColor * ambientColor;
	float diffuse = max(0,dot(N,L)) * diffuseColor * diffuseLightColor;
	float3 H = normalize(L + V);
	float specular = pow(max(0, dot(N,H)), specularColor.w)
		* float4(specularColor.xyz, 1) * specularLightColor;

	return texColor * (ambient + diffuse) + specular + emissiveColor;
}